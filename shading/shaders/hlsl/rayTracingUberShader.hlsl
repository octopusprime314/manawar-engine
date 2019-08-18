#define USE_SHADER_MODEL_6_5 0

struct SceneConstantBuffer
{
    float4x4 projectionToWorld;
    float4   cameraPosition;
    float4   lightPosition;
    float4   lightDirection;
    float4x4 projection;
    float4x4 lightView;
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
struct RayPayload
{
    float4 color;
};

typedef BuiltInTriangleIntersectionAttributes MyAttributes;

RaytracingAccelerationStructure Scene       : register(t0, space0);
RWTexture2D<float4> RenderTarget            : register(u0);
ByteAddressBuffer Indices                   : register(t1, space0);
StructuredBuffer<Vertex> Vertices           : register(t2, space0);
ConstantBuffer<SceneConstantBuffer> sceneCB : register(b0);


// Retrieve hit world position.
float3 HitWorldPosition()
{
    return WorldRayOrigin() +
           RayTCurrent()    *
           WorldRayDirection();
}

// Retrieve attribute at a hit position interpolated from vertex attributes using the hit's barycentrics.
float3 HitAttribute(float3                                vertexAttribute[3],
                    BuiltInTriangleIntersectionAttributes attr)
{
    return vertexAttribute[0] +
           attr.barycentrics.x * (vertexAttribute[1] - vertexAttribute[0]) +
           attr.barycentrics.y * (vertexAttribute[2] - vertexAttribute[0]);
}

// Generate a ray in world space for a camera pixel corresponding to an index from the dispatched 2D grid.
inline void GenerateCameraRay(    uint2  index,
                              out float3 origin,
                              out float3 direction)
{
    float2 xy = index + 0.5f; // center in the middle of the pixel.
    float2 screenPos = xy / DispatchRaysDimensions().xy * 2.0 - 1.0;

    // Invert Y for DirectX-style coordinates.
    screenPos.y = -screenPos.y;

    // Unproject the pixel coordinate into a ray.
    float4 world = mul(float4(screenPos, 0, 1), sceneCB.projectionToWorld);

    world.xyz /= world.w;
    //Projection matrix
    //origin = sceneCB.cameraPosition.xyz;
    //direction = normalize(world.xyz - origin);

    //Orthographic matrix
    origin = world.xyz;
    direction = sceneCB.lightDirection.xyz;
}

[shader("raygeneration")]
void MyRaygenShader()
{
    float3 rayDir;
    float3 origin;

    GenerateCameraRay(DispatchRaysIndex(), origin, rayDir);

    // Trace the ray.
    // Set the ray's extents.
    RayDesc ray;
    ray.Origin         = origin;
    ray.Direction      = rayDir;

    // Set TMin to a non-zero small val(ue to avoid aliasing issues due to floating - point errors.
    // TMin should be kept small to prevent missing geometry at close contact areas.
    ray.TMin           = 0.001;
    ray.TMax           = 10000.0;
    RayPayload payload = { float4(0, 0, 0, 0) };

#if (USE_SHADER_MODEL_6_5 == 1)
    RayQuery<RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH> rayQuery;
    rayQuery.TraceRayInline(Scene,
                            RAY_FLAG_NONE,
                            ~0,
                            ray);

    if (rayQuery.Proceed() == false && rayQuery.CommittedStatus() == COMMITTED_TRIANGLE_HIT)
    {
        float3   hitPosition   = rayQuery.WorldRayOrigin() +
                                 (rayQuery.CommittedRayT() * rayQuery.WorldRayDirection());

        float4x4 lightViewProj = mul(sceneCB.lightView, sceneCB.projection);
        float4   clipSpace     = mul(float4(hitPosition, 1), lightViewProj);
        float    depth         = clipSpace.z;

        float3x4 data          = rayQuery.CandidateWorldToObject3x4();
        data                   = mul(data, rayQuery.CandidateObjectToWorld3x4());
        data                   = mul(data, rayQuery.CommittedObjectToWorld3x4());
        data                   = mul(data, rayQuery.CommittedWorldToObject3x4());

        if (data[0][0] > 0.0f)
        {
            payload.color += float4(depth, depth, depth, 1.0f);
        }
        else
        {
            payload.color += float4(1.0, 0.0, 0.0, 1.0f);
        }

    }
    else
    {
        payload.color += float4(1.0f, 0.0f, 0.0f, 0.0f);
    }

    //RayQuery<RAY_FLAG_NONE> rayQuery2;
    //rayQuery2.TraceRayInline(Scene,
    //                        RAY_FLAG_NONE,
    //                        ~0,
    //                        ray);
    //
    //rayQuery2.Proceed();
    //
    //if (rayQuery2.CommittedStatus() == COMMITTED_TRIANGLE_HIT)
    //{
    //    float3   hitPosition   = rayQuery2.WorldRayOrigin() +
    //                             //(rayQuery.RayTCurrent() *
    //                             (rayQuery2.CommittedRayT() *
    //                                 rayQuery2.WorldRayDirection());
    //
    //    float4x4 lightViewProj = mul(sceneCB.lightView, sceneCB.projection);
    //    float4   clipSpace     = mul(float4(hitPosition, 1), lightViewProj);
    //    float    depth         = clipSpace.z;
    //
    //    payload.color += float4(depth, depth, depth, 1.0f);
    //}
    //else
    //{
    //    payload.color += float4(1.0f, 0.0f, 0.0f, 0.0f);
    //}
#else
    TraceRay(Scene, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, ~0, 0, 1, 0, ray, payload);

#endif

    // Write the raytraced color to the output texture.
    RenderTarget[DispatchRaysIndex().xy] = payload.color;
}

[shader("closesthit")]
void MyClosestHitShader(inout RayPayload   payload,
                        in    MyAttributes attr)
{
    float3   hitPosition   = HitWorldPosition();
    float4x4 lightViewProj = mul(sceneCB.lightView, sceneCB.projection);
    float4   clipSpace     = mul(float4(hitPosition, 1), lightViewProj);
    payload.color          = float4(clipSpace.z, clipSpace.z, clipSpace.z, 1.0);
}

[shader("miss")]
void MyMissShader(inout RayPayload payload)
{
    float4 background = float4(1.0f, 0.0f, 0.0f, 0.0f);
    payload.color     = background;

}
