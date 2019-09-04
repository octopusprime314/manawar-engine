#define USE_SHADER_MODEL_6_5 1

// Object Declarations

Texture2D   diffuseTexture     : register(t0); // Diffuse texture data array
Texture2D   normalTexture      : register(t1); // Normal texture data array
Texture2D   velocityTexture    : register(t2); // Velocity texture data array
Texture2D   depthTexture       : register(t3); // Depth texture data array
Texture2D   cameraDepthTexture : register(t4); // Depth texture data array
Texture2D   mapDepthTexture    : register(t5); // Depth texture data array
TextureCube depthMap           : register(t6); // Cube depth map for point light shadows
TextureCube skyboxDayTexture   : register(t7); // Skybox day
TextureCube skyboxNightTexture : register(t8); // Skybox night
Texture2D   ssaoTexture        : register(t9); // Depth texture data array
sampler     textureSampler     : register(s0);

#if (USE_SHADER_MODEL_6_5 == 1)
//Raytracing Acceleration Structure
RaytracingAccelerationStructure rtAS : register(t10);
Texture2D   transparencyTexture1     : register(t11); // transparency texture 1
Texture2D   transparencyTexture2     : register(t12); // transparency texture 2
Texture2D   transparencyTexture3     : register(t13); // transparency texture 3
Texture2D   transparencyTexture4     : register(t14); // transparency texture 4
Texture2D   transparencyTexture5     : register(t15); // transparency texture 5

#endif

cbuffer globalData             : register(b0) {
    float4x4 lightViewMatrix;                  // Light perspective's view matrix
    float4x4 lightProjectionMatrix;            // Light perspective's view matrix
    float4x4 inverseView;                      // Light perspective's view matrix
    float4x4 lightMapViewMatrix;               // Light perspective's view matrix
    float4x4 viewToModelMatrix;                // Inverse camera view space matrix
    float4x4 projectionToViewMatrix;           // Inverse projection matrix
    float4x4 normalMatrix;                     // inverse transpose of view matrix
    float3   pointLightPositions[20];          // Max lights is 20 for now
    float3   pointLightColors[20];             // Max lights is 20 for now
    float    pointLightRanges[20];             // Max lights is 20 for now
    int      numPointLights;
    int      views;                            // views set to 0 is diffuse mapping,
                                               // set to 1 is shadow mapping
                                               // and set to 2 is normal mapping
    float3   lightDirection;
    float4x4 lightRayProjection;               // tracerayinline light projection matrix
}

static const float2 poissonDisk[4] = {
    float2(-0.94201624,  -0.39906216),
    float2( 0.94558609,  -0.76890725),
    float2(-0.094184101, -0.92938870),
    float2( 0.34495938,   0.29387760)
};

static const float shadowEffect = 0.6;
static const float ambient      = 0.3;

float3 decodeLocation(float2 uv) {
    float4 clipSpaceLocation;
    clipSpaceLocation.xy      = mul(uv, 2.0f) - 1.0f;
    //TODO: need to fix cpu
    clipSpaceLocation.y       = -clipSpaceLocation.y;
    //dx z clip space is [0,1]
    clipSpaceLocation.z       = depthTexture.Sample(textureSampler, uv).r;
    clipSpaceLocation.w       = 1.0f;
    float4 homogenousLocation = mul(clipSpaceLocation, projectionToViewMatrix);
    return homogenousLocation.xyz / homogenousLocation.w;
}

void VS(    uint   id    : SV_VERTEXID,
        out float4 oPosH : SV_POSITION,
        out float2 oUV   : UVOUT) {

    oPosH.x = (float)(id / 2) * 4.0 - 1.0;
    oPosH.y = (float)(id % 2) * 4.0 - 1.0;
    oPosH.z = 0.0;
    oPosH.w = 1.0;
    oUV.x   =       (float)(id / 2) * 2.0;
    oUV.y   = 1.0 - (float)(id % 2) * 2.0;
}
struct PixelOut
{
    float4 color  : SV_Target0;
    float4 debug0 : SV_Target1;
    float4 debug1 : SV_Target2;
    float  depth  : SV_Depth;
};

// Generate a ray in world space for a camera pixel corresponding to an index from the dispatched 2D grid.
void GenerateCameraRay(    float2 uv,
                       out float3 origin,
                       out float3 direction)
{
    float2 screenPos = (2.0f * uv) - 1.0f;

    // Invert Y for DirectX-style coordinates.
    screenPos.y = -screenPos.y;

    // Unproject the pixel coordinate into a ray.
    float4 world     = mul(float4(screenPos, 0, 1), lightRayProjection);
    world.xyz       /= world.w;

    //Orthographic matrix
    origin           = world.xyz;
    direction        = lightDirection.xyz;
}

PixelOut PS(float4 posH : SV_POSITION,
            float2 uv   : UVOUT) {


    const float bias = 0.005; //removes shadow acne by adding a small bias

    PixelOut pixel   = { float4(0.0, 0.0, 0.0, 0.0),
                         float4(0.0, 0.0, 0.0, 0.0),
                         float4(0.0, 0.0, 0.0, 0.0),
                         1.0 };

    //extract position from depth texture
    float4 position         = float4(decodeLocation(uv), 1.0);
    //extract normal from normal texture
    float4 normal           = normalTexture.Sample(textureSampler, uv);
    float3 normalizedNormal = normalize(normal.xyz);
    //extract color from diffuse texture
    float4 diffuse          = diffuseTexture.Sample(textureSampler, uv);
    //extract 2d velocity buffer
    float2 velocity         = velocityTexture.Sample(textureSampler, uv).rg;
    float occlusion         = ssaoTexture.Sample(textureSampler, uv).r;
    //blit depth
    pixel.depth             = depthTexture.Sample(textureSampler, uv).r;

    //Directional light calculation
    float3 normalizedLight   = normalize(lightDirection);
    float3 lightInCameraView = normalize(float3(mul(float4(-normalizedLight, 0.0), normalMatrix).xyz));
    float  illumination      = dot(lightInCameraView, normalizedNormal);

    float4x4 inverseToLightView         = mul(inverseView, lightViewMatrix);
    float4x4 inverseLightViewProjection = mul(inverseToLightView, lightProjectionMatrix);

    //Convert from camera space vertex to light clip space vertex
    float4 shadowMapping                = mul(float4(position.xyz, 1.0), inverseLightViewProjection);
    shadowMapping                       = shadowMapping / shadowMapping.w;
    float2 shadowTextureCoordinates     = mul(shadowMapping.xy, 0.5) + float2(0.5, 0.5);
                                        
    float4 shadowMappingMap             = mul(float4(position.xyz, 1.0), lightMapViewMatrix);
    shadowMappingMap                    = shadowMappingMap / shadowMappingMap.w;
    float2 shadowTextureCoordinatesMap  = mul(shadowMappingMap.xy, 0.5) + float2(0.5,0.5);

    //TODO: need to fix cpu
    float2 invertedYCoord = float2(shadowTextureCoordinates.x, -shadowTextureCoordinates.y);

    const float MAX_DEPTH   = 10000.0;
    float       rtDepth     = MAX_DEPTH;
    float directionalShadow = 1.0;

    float3 rayDir;
    float3 origin;

    // Why does the y component of the shadow texture mapping need to be 1.0 - yCoord?
    GenerateCameraRay(float2(shadowTextureCoordinates.x, 1.0 - shadowTextureCoordinates.y),
                      origin,
                      rayDir);

#if (USE_SHADER_MODEL_6_5 == 1)


    // Trace the ray.
    // Set the ray's extents.
    RayDesc ray;
    ray.Origin    = origin;
    ray.Direction = rayDir;
    ray.TMin      = 0.001;
    ray.TMax      = MAX_DEPTH;

    RayQuery<RAY_FLAG_NONE> rayQuery;
    rayQuery.TraceRayInline(rtAS,
                            RAY_FLAG_NONE,
                            ~0,
                            ray);

    //Transparency testing
    while (rayQuery.Proceed() == true)
    {
        if (rayQuery.CandidateType() == CANDIDATE_NON_OPAQUE_TRIANGLE) {

            float  alphaValue   = 1.0f;
            float2 barycentrics = rayQuery.CandidateTriangleBarycentrics();
            barycentrics = float2(-barycentrics.x, -barycentrics.y);
            if (rayQuery.CandidateInstanceID() == 1) {
                alphaValue = transparencyTexture1.Sample(textureSampler, barycentrics).a;
            }
            else if (rayQuery.CandidateInstanceID() == 2) {
                alphaValue = transparencyTexture2.Sample(textureSampler, barycentrics).a;
            }
            else if (rayQuery.CandidateInstanceID() == 3) {
                alphaValue = transparencyTexture3.Sample(textureSampler, barycentrics).a;
            }
            else if (rayQuery.CandidateInstanceID() == 4) {
                alphaValue = transparencyTexture4.Sample(textureSampler, barycentrics).a;
            }
            else if (rayQuery.CandidateInstanceID() == 5) {
                alphaValue = transparencyTexture5.Sample(textureSampler, barycentrics).a;
            }

            if (alphaValue > 0.1) {
                rayQuery.CommitNonOpaqueTriangleHit();
            }

            /*if (rayQuery.CandidateInstanceID() == 5) {
                if (transparencyTexture1.Sample(textureSampler, rayQuery.CandidateTriangleBarycentrics()).a > 0.1) {
                    rayQuery.CommitNonOpaqueTriangleHit();
                }
            }*/
        }
    }
    if (rayQuery.CommittedStatus() == COMMITTED_TRIANGLE_HIT)
    {
        float3   hitPosition   = rayQuery.WorldRayOrigin() +
                                (rayQuery.CommittedRayT() * rayQuery.WorldRayDirection());

        float4x4 lightViewProj = mul(lightViewMatrix, lightProjectionMatrix);
        float4   clipSpace     = mul(float4(hitPosition, 1), lightViewProj);
        rtDepth                = clipSpace.z;
    }

    //Opaque geometry
    /*if (rayQuery.Proceed()         == false &&
        rayQuery.CommittedStatus() == COMMITTED_TRIANGLE_HIT)
    {
        float3   hitPosition   = rayQuery.WorldRayOrigin() +
                                 (rayQuery.CommittedRayT() * rayQuery.WorldRayDirection());

        float4x4 lightViewProj = mul(lightViewMatrix, lightProjectionMatrix);
        float4   clipSpace     = mul(float4(hitPosition, 1), lightViewProj);
        rtDepth                = clipSpace.z;

        if (transparencyTexture.Sample(textureSampler, rayQuery.CommittedTriangleBarycentrics()).a <= 0.1) {
            rtDepth = MAX_DEPTH;
        }
    }*/
#endif

    if (views == 0) {
        //Detects if there is no screen space information and then displays skybox!
        if (normal.x == 0.0 &&
            normal.y == 0.0 &&
            normal.z == 0.0) {
            float4 dayColor   = skyboxDayTexture.Sample(textureSampler,
                                                        float3(position.x, -position.y, position.z));
            float4 nightColor = skyboxNightTexture.Sample(textureSampler,
                                                          float3(position.x, -position.y, position.z));
            pixel.color       = (((1.0 - normalizedLight.y) / 2.0) * dayColor) +
                                (((1.0 + normalizedLight.y) / 2.0) * nightColor);
            //skybox depth trick to have it displayed at the depth boundary
            //precision matters here and must be as close as possible to 1.0
            //the number of 9s can only go to 7 but no less than 4
            pixel.depth       = 0.9999999;
        }
        else {
            float3 pointLighting    = float3(0.0, 0.0, 0.0);
            float totalShadow       = 1.0;
            float pointShadow       = 1.0;

#if (USE_SHADER_MODEL_6_5 == 0)

            float d = cameraDepthTexture.Sample(textureSampler, invertedYCoord).r;
#else
            float d = rtDepth;
#endif
            //illumination is from directional light but we don't want to illuminate when the sun is past the horizon
            //aka night time
            if (normalizedLight.y <= 0.0) {
                //Only shadow in textures space
                if (shadowTextureCoordinates.x <= 1.0 &&
                    shadowTextureCoordinates.x >= 0.0 &&
                    shadowTextureCoordinates.y <= 1.0 &&
                    shadowTextureCoordinates.y >= 0.0) {

                    if (d < shadowMapping.z - bias) {
                        directionalShadow = shadowEffect;
                    }
                }
            }
            else {
                illumination = 0.0;
            }

            //Point lights always emit light versus directional sun shadows
            float numLights              = numPointLights;
            float totalPointLightEffect  = 0.0;
            for (int i = 0; i < numPointLights; i++) {
                float3 pointLightDir     = position.xyz - pointLightPositions[i].xyz;
                float  distanceFromLight = length(pointLightDir);
                float  bias              = 0.1;
                if (distanceFromLight < pointLightRanges[i]) {
                    float3 pointLightDirNorm = normalize(-pointLightDir);
                    pointLighting           += (dot(pointLightDirNorm, normalizedNormal)) *
                                               (1.0 - (distanceFromLight / (pointLightRanges[i]))) * pointLightColors[i];
                    totalPointLightEffect   += dot(pointLightDirNorm, normalizedNormal) *
                                               (1.0 - (distanceFromLight / (pointLightRanges[i])));

                    float3 cubeMapTexCoords  = mul(float4(position.xyz,1.0), viewToModelMatrix).xyz -
                                                                             mul(float4(pointLightPositions[i].xyz, 1.0),
                                                                                 viewToModelMatrix).xyz;
                    float distance           = length(cubeMapTexCoords);
                    float cubeDepth          = depthMap.Sample(textureSampler, normalize(cubeMapTexCoords.xyz)).x*pointLightRanges[i];

                    if (cubeDepth + bias < distance) {
                        pointShadow         -= (1.0 - shadowEffect);
                    }
                }
            }

            totalShadow                       = min(directionalShadow, pointShadow);
            float3 lightComponentIllumination = (illumination  * diffuse.rgb * directionalShadow) +
                                                (pointLighting * diffuse.rgb * pointShadow);
            pixel.color                       = float4((illumination * diffuse.rgb * directionalShadow), 1.0);
        }
    }
    else if (views == 1) {
        pixel.color = float4(diffuse.rgb, 1.0);
    }
    else if (views == 2) {
        pixel.color = float4(normalizedNormal.xyz, 1.0);
    }
    else if (views == 3) {
        pixel.color = float4(normalize(position.xyz), 1.0);
    }
    else if (views == 4) {
        pixel.color = float4(float2(abs(velocity.r), abs(velocity.g)), 0.0, 1.0);
    }
    else if (views == 5) {
        pixel.color = float4(occlusion, occlusion, occlusion, 1.0);
    }
    else if (views == 6) {
        float depth = cameraDepthTexture.Sample(textureSampler, uv).x;
        pixel.color = float4(depth, depth, depth, 1.0);
        pixel.depth = 0.1;
    }
    else if (views == 7) {
        float2 screenPos = (2.0f * uv) - 1.0f;
        // Invert Y for DirectX-style coordinates.
        screenPos.y = -screenPos.y;
        //pixel.color = float4(screenPos.x, screenPos.y, 0.0, 1.0);
        
        float depth = rtDepth;
        pixel.color = float4(depth, depth, depth, 1.0);
        pixel.depth = 0.1;

        pixel.debug0 = float4(float3(origin.xyz), 1.0);
        pixel.debug1 = float4(float3(rayDir.xyz), 1.0);

        //pixel.color = float4(float3(abs(origin.xyz)), 1.0);
        //pixel.depth = abs(origin.x);
    }
    else if (views == 8) {
        float3 cubeMapTexCoords = mul(float4(position.xyz,1.0), viewToModelMatrix).xyz -
                                  mul(float4(pointLightPositions[0].xyz, 1.0), viewToModelMatrix).xyz;
        float cubeDepth         = depthMap.Sample(textureSampler, normalize(cubeMapTexCoords.xyz)).x;
        pixel.color             = float4(cubeDepth, cubeDepth, cubeDepth, 1.0);
        pixel.depth             = 0.1;
    }
    else if (views == 9) {
        float depth = mapDepthTexture.Sample(textureSampler, uv).x;
        pixel.color = float4(depth, depth, depth, 1.0);
        pixel.depth = 0.1;
    }
    return pixel;
}