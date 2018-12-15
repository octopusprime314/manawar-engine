//struct Viewport
//{
//    float left;
//    float top;
//    float right;
//    float bottom;
//};
//
//struct RayGenConstantBuffer
//{
//    Viewport viewport;
//    Viewport stencil;
//};
//
//RaytracingAccelerationStructure Scene : register(t0, space0);
//RWTexture2D<float4> RenderTarget : register(u0);
//ConstantBuffer<RayGenConstantBuffer> g_rayGenCB : register(b0);
//
//typedef BuiltInTriangleIntersectionAttributes MyAttributes;
//struct RayPayload {
//    float4 color;
//};
//
//bool IsInsideViewport(float2 p, Viewport viewport) {
//    return (p.x >= viewport.left && p.x <= viewport.right)
//        && (p.y >= viewport.top && p.y <= viewport.bottom);
//}
//
//[shader("raygeneration")]
//void MyRaygenShader() {
//    float2 lerpValues = (float2)DispatchRaysIndex() / (float2)DispatchRaysDimensions();
//
//    // Orthographic projection since we're raytracing in screen space.
//    float3 rayDir = float3(0, 0, 1);
//    float3 origin = float3(
//        lerp(g_rayGenCB.viewport.left, g_rayGenCB.viewport.right, lerpValues.x),
//        lerp(g_rayGenCB.viewport.top, g_rayGenCB.viewport.bottom, lerpValues.y),
//        0.0f);
//
//    if (IsInsideViewport(origin.xy, g_rayGenCB.stencil))
//    {
//        // Trace the ray.
//        // Set the ray's extents.
//        RayDesc ray;
//        ray.Origin = origin;
//        ray.Direction = rayDir;
//        // Set TMin to a non-zero small value to avoid aliasing issues due to floating - point errors.
//        // TMin should be kept small to prevent missing geometry at close contact areas.
//        ray.TMin = 0.001;
//        ray.TMax = 10000.0;
//        RayPayload payload = { float4(0, 0, 0, 0) };
//        TraceRay(Scene, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, ~0, 0, 1, 0, ray, payload);
//
//        // Write the raytraced color to the output texture.
//        RenderTarget[DispatchRaysIndex().xy] = payload.color;
//    }
//    else
//    {
//        // Render interpolated DispatchRaysIndex outside the stencil window
//        RenderTarget[DispatchRaysIndex().xy] = float4(lerpValues, 0, 1);
//    }
//}
//
//[shader("closesthit")]
//void MyClosestHitShader(inout RayPayload payload, in MyAttributes attr) {
//    float3 barycentrics = float3(1 - attr.barycentrics.x - attr.barycentrics.y, attr.barycentrics.x, attr.barycentrics.y);
//    payload.color = float4(barycentrics, 1);
//}
//
//[shader("miss")]
//void MyMissShader(inout RayPayload payload) {
//    payload.color = float4(0, 0, 0, 1);
//}

struct SceneConstantBuffer
{
    float4x4 projectionToWorld;
    float4 cameraPosition;
    float4 lightPosition;
    float4 lightAmbientColor;
    float4 lightDiffuseColor;
};

struct CubeConstantBuffer
{
    float4 albedo;
};

struct Vertex
{
    float3 position;
    float3 normal;
};


struct Viewport
{
    float left;
    float top;
    float right;
    float bottom;
};

struct RayGenConstantBuffer
{
    Viewport viewport;
    Viewport stencil;
};

RaytracingAccelerationStructure Scene : register(t0, space0);
RWTexture2D<float4> RenderTarget : register(u0);
ByteAddressBuffer Indices : register(t1, space0);
StructuredBuffer<Vertex> Vertices : register(t2, space0);

ConstantBuffer<SceneConstantBuffer> g_sceneCB : register(b0);
ConstantBuffer<CubeConstantBuffer> g_cubeCB : register(b1);

// Load three 16 bit indices from a byte addressed buffer.
uint3 Load3x16BitIndices(uint offsetBytes)
{
    uint3 indices;

    // ByteAdressBuffer loads must be aligned at a 4 byte boundary.
    // Since we need to read three 16 bit indices: { 0, 1, 2 } 
    // aligned at a 4 byte boundary as: { 0 1 } { 2 0 } { 1 2 } { 0 1 } ...
    // we will load 8 bytes (~ 4 indices { a b | c d }) to handle two possible index triplet layouts,
    // based on first index's offsetBytes being aligned at the 4 byte boundary or not:
    //  Aligned:     { 0 1 | 2 - }
    //  Not aligned: { - 0 | 1 2 }
    const uint dwordAlignedOffset = offsetBytes & ~3;
    const uint2 four16BitIndices = Indices.Load2(dwordAlignedOffset);

    // Aligned: { 0 1 | 2 - } => retrieve first three 16bit indices
    if (dwordAlignedOffset == offsetBytes)
    {
        indices.x = four16BitIndices.x & 0xffff;
        indices.y = (four16BitIndices.x >> 16) & 0xffff;
        indices.z = four16BitIndices.y & 0xffff;
    }
    else // Not aligned: { - 0 | 1 2 } => retrieve last three 16bit indices
    {
        indices.x = (four16BitIndices.x >> 16) & 0xffff;
        indices.y = four16BitIndices.y & 0xffff;
        indices.z = (four16BitIndices.y >> 16) & 0xffff;
    }

    return indices;
}

typedef BuiltInTriangleIntersectionAttributes MyAttributes;
struct RayPayload
{
    float4 color;
};

// Retrieve hit world position.
float3 HitWorldPosition()
{
    return WorldRayOrigin() + RayTCurrent() * WorldRayDirection();
}

// Retrieve attribute at a hit position interpolated from vertex attributes using the hit's barycentrics.
float3 HitAttribute(float3 vertexAttribute[3], BuiltInTriangleIntersectionAttributes attr)
{
    return vertexAttribute[0] +
        attr.barycentrics.x * (vertexAttribute[1] - vertexAttribute[0]) +
        attr.barycentrics.y * (vertexAttribute[2] - vertexAttribute[0]);
}

// Generate a ray in world space for a camera pixel corresponding to an index from the dispatched 2D grid.
inline void GenerateCameraRay(uint2 index, out float3 origin, out float3 direction)
{
    float2 xy = index + 0.5f; // center in the middle of the pixel.
    float2 screenPos = xy / DispatchRaysDimensions().xy * 2.0 - 1.0;

    // Invert Y for DirectX-style coordinates.
    screenPos.y = -screenPos.y;

    // Unproject the pixel coordinate into a ray.
    float4 world = mul(float4(screenPos, 0, 1), g_sceneCB.projectionToWorld);

    world.xyz /= world.w;
    origin = g_sceneCB.cameraPosition.xyz;
    direction = normalize(world.xyz - origin);
}

// Diffuse lighting calculation.
float4 CalculateDiffuseLighting(float3 hitPosition, float3 normal)
{
    float3 pixelToLight = normalize(g_sceneCB.lightPosition.xyz - hitPosition);

    // Diffuse contribution.
    float fNDotL = max(0.0f, dot(pixelToLight, normal));

    return g_cubeCB.albedo * g_sceneCB.lightDiffuseColor * fNDotL;
}

[shader("raygeneration")]
void MyRaygenShader()
{
    float3 rayDir;
    float3 origin;

    float2 xy = DispatchRaysIndex().xy + 0.5f; // center in the middle of the pixel.
    float2 screenPos = xy / DispatchRaysDimensions().xy * 2.0 - 1.0;

    // Invert Y for DirectX-style coordinates.
    screenPos.y = -screenPos.y;

    // Unproject the pixel coordinate into a ray.
    float4 world = mul(float4(screenPos, 0, 1), g_sceneCB.projectionToWorld);

    world.xyz /= world.w;
    origin = g_sceneCB.cameraPosition.xyz;
    rayDir = normalize(world.xyz - origin);

    // Trace the ray.
    // Set the ray's extents.
    RayDesc ray;
    ray.Origin = origin;
    ray.Direction = rayDir;

    // Set TMin to a non-zero small val(ue to avoid aliasing issues due to floating - point errors.
    // TMin should be kept small to prevent missing geometry at close contact areas.
    ray.TMin = 0.001;
    ray.TMax = 10000.0;
    RayPayload payload = { float4(0, 0, 0, 0) };
    TraceRay(Scene, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, ~0, 0, 1, 0, ray, payload);

    // Write the raytraced color to the output texture.
    RenderTarget[DispatchRaysIndex().xy] = payload.color;
}

[shader("closesthit")]
void MyClosestHitShader(inout RayPayload payload, in MyAttributes attr)
{
    float3 rayDir;
    float3 origin;

    float2 xy = DispatchRaysIndex().xy + 0.5f; // center in the middle of the pixel.
    float2 screenPos = xy / DispatchRaysDimensions().xy * 2.0 - 1.0;

    // Invert Y for DirectX-style coordinates.
    screenPos.y = -screenPos.y;

    // Unproject the pixel coordinate into a ray.
    float4 world = mul(float4(screenPos, 0, 1), g_sceneCB.projectionToWorld);

    world.xyz /= world.w;

    //ray.Origin = float4(0.0, 1.0, 0.0, 1.0);
    payload.color = float4(world.zzz, 1.0f);
    //payload.color = float4(abs(rayDir.zzz), 1.0f);
    //payload.color = float4(abs(screenPos), 0.0, 1.0f);

}

[shader("miss")]
void MyMissShader(inout RayPayload payload)
{

    float3 rayDir;
    float3 origin;

    float4 background = float4(1.0f, 0.0f, 0.0f, 1.0f);
    payload.color = background;
    //payload.color = float4(normalize(rayDir), 1.0);
    
    
    //float2 xy = DispatchRaysIndex().xy + 0.5f; // center in the middle of the pixel.
    //float2 screenPos = xy / DispatchRaysDimensions().xy * 2.0 - 1.0;

    //// Invert Y for DirectX-style coordinates.
    //screenPos.y = -screenPos.y;

    //// Unproject the pixel coordinate into a ray.
    //float4 world = mul(float4(screenPos, 0, 1), g_sceneCB.projectionToWorld);

    //world.xyz /= world.w;
    //origin = g_sceneCB.cameraPosition.xyz;
    //float3 direction = normalize(world.xyz - origin);
    //payload.color = float4(abs(direction), 1.0);

}
