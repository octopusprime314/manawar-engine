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
struct Vertex {
    float3 pos;
    float3 normal;
    float2 uv;
};
// Raytracing Acceleration Structure
RaytracingAccelerationStructure rtAS                 : register(t10);
Texture2D                       transparencyTexture1 : register(t11); // transparency texture 1
StructuredBuffer<Vertex>        vertexBuffer        : register(t12); // UV coordinates to shade reflections
//Texture2D                       transparencyTexture2 : register(t13); // transparency texture 2
//Texture2D                       transparencyTexture3 : register(t14); // transparency texture 3
//Texture2D                       transparencyTexture4 : register(t15); // transparency texture 4
//Texture2D                       transparencyTexture5 : register(t16); // transparency texture 5
//Texture2D                       materialTextures[16] : register(t17); // All materials to shade reflections

#define USE_FIRST_HIT_END_SEARCH_SHADOWS 1
#endif

cbuffer globalData : register(b0) {
    int      views;
    float4x4 inverseView;
    float4x4 normalMatrix;
    int      numPointLights;
    float3   lightDirection;
    float4x4 lightViewMatrix;
    float4x4 viewToModelMatrix;
    float4x4 lightRayProjection;
    float4x4 lightMapViewMatrix;
    float4x4 viewProjection;
    float3   pointLightColors[20];
    float    pointLightRanges[20];
    float4x4 lightProjectionMatrix;
    float4x4 projectionToViewMatrix;
    float3   pointLightPositions[20];
}

static const float2 poissonDisk[4] = { float2(-0.94201624,  -0.39906216),
                                       float2( 0.94558609,  -0.76890725),
                                       float2(-0.094184101, -0.92938870),
                                       float2( 0.34495938,   0.29387760) };

static const float shadowEffect = 0.6;
static const float ambient      = 0.3;

float3 decodeLocation(float2 uv) {
    float4 clipSpaceLocation;
    clipSpaceLocation.xy = mul(uv, 2.0f) - 1.0f;
    // TODO: need to fix cpu
    clipSpaceLocation.y = -clipSpaceLocation.y;
    // dx z clip space is [0,1]
    clipSpaceLocation.z       = depthTexture.Sample(textureSampler, uv).r;
    clipSpaceLocation.w       = 1.0f;
    float4 homogenousLocation = mul(clipSpaceLocation, projectionToViewMatrix);
    return homogenousLocation.xyz / homogenousLocation.w;
}

void VS(uint id : SV_VERTEXID, out float4 outPosition : SV_POSITION, out float2 outUV : UVOUT) {

    outPosition.x = (float)(id / 2) * 4.0 - 1.0;
    outPosition.y = (float)(id % 2) * 4.0 - 1.0;
    outPosition.z = 0.0;
    outPosition.w = 1.0;
    outUV.x       = (float)(id / 2) * 2.0;
    outUV.y       = 1.0 - (float)(id % 2) * 2.0;
}
struct PixelOut {
    float4 color : SV_Target0;
    float4 debug0 : SV_Target1;
    float4 debug1 : SV_Target2;
    float  depth : SV_Depth;
};

// Generate a ray in world space for a camera pixel corresponding to an index from the dispatched 2D grid.
void GenerateCameraRay(float2 uv, out float3 origin, out float3 direction) {
    float2 screenPos = (2.0f * uv) - 1.0f;

    // Invert Y for DirectX-style coordinates.
    screenPos.y = -screenPos.y;

    // Unproject the pixel coordinate into a ray.
    float4 world = mul(float4(screenPos, 0, 1), lightRayProjection);
    world.xyz /= world.w;

    // Orthographic matrix
    origin    = world.xyz;
    direction = lightDirection.xyz;
}

float2 GetTexCoord(float2 barycentrics, uint instanceContribution, uint primitiveIndex) {

    float2 texCoord[3];

    texCoord[0] = vertexBuffer.Load((primitiveIndex * 3) + 0).uv;
    texCoord[1] = vertexBuffer.Load((primitiveIndex * 3) + 1).uv;
    texCoord[2] = vertexBuffer.Load((primitiveIndex * 3) + 2).uv;

    return (texCoord[0]                                  +
            barycentrics.x * (texCoord[1] - texCoord[0]) +
            barycentrics.y * (texCoord[2] - texCoord[0]));
}

float3 GetNormal(uint primitiveIndex) {

    float3 normal[3];

    normal[0] = vertexBuffer.Load((primitiveIndex * 3) + 0).normal;
    normal[1] = vertexBuffer.Load((primitiveIndex * 3) + 1).normal;
    normal[2] = vertexBuffer.Load((primitiveIndex * 3) + 2).normal;

    // average the normals maybe or take one of the normals?
    //float3 averagedNormal = (normal[0] + normal[1] + normal[2]) / 3.0f;
    float3 averagedNormal = normal[0];
    return averagedNormal;
}

PixelOut PS(float4 posH : SV_POSITION, float2 uv : UVOUT) {

    const float bias = 0.005; // removes shadow acne by adding a small bias

    PixelOut pixel = {float4(0.0, 0.0, 0.0, 0.0),
                      float4(0.0, 0.0, 0.0, 0.0),
                      float4(0.0, 0.0, 0.0, 0.0),
                      1.0};

    // extract position from depth texture
    float4 position         = float4(decodeLocation(uv), 1.0);
    // extract normal from normal texture
    float4 normal           = normalTexture.Sample(textureSampler, uv);
    float3 normalizedNormal = normalize(normal.xyz);
    // extract color from diffuse texture
    float4 diffuse          = diffuseTexture.Sample(textureSampler, uv);
    // extract 2d velocity buffer
    float2 velocity         = velocityTexture.Sample(textureSampler, uv).rg;
    float  occlusion        = ssaoTexture.Sample(textureSampler, uv).r;
    // blit depth
    pixel.depth             = depthTexture.Sample(textureSampler, uv).r;

    // Directional light calculation
    float3 normalizedLight   = normalize(lightDirection);
    float3 lightInCameraView = normalize(float3(mul(float4(-normalizedLight, 0.0), normalMatrix).xyz));
    float  illumination      = dot(lightInCameraView, normalizedNormal);

    float4x4 inverseToLightView         = mul(inverseView, lightViewMatrix);
    float4x4 inverseLightViewProjection = mul(inverseToLightView, lightProjectionMatrix);

    // Convert from camera space vertex to light clip space vertex
    float4 shadowMapping            = mul(float4(position.xyz, 1.0), inverseLightViewProjection);
    shadowMapping                   = shadowMapping / shadowMapping.w;
    float2 shadowTextureCoordinates = mul(shadowMapping.xy, 0.5) + float2(0.5, 0.5);

    float4 shadowMappingMap            = mul(float4(position.xyz, 1.0), lightMapViewMatrix);
    shadowMappingMap                   = shadowMappingMap / shadowMappingMap.w;
    float2 shadowTextureCoordinatesMap = mul(shadowMappingMap.xy, 0.5) + float2(0.5, 0.5);

    // TODO: need to fix cpu
    float2 invertedYCoord = float2(shadowTextureCoordinates.x, -shadowTextureCoordinates.y);

    const float MAX_DEPTH         = 10000.0;
    float       rtDepth           = MAX_DEPTH;
    float       directionalShadow = 1.0;

    float3 rayDir;
    float3 origin;
    float  rtOcclusion  = 1.0;
    float3 rtReflection = float3(0.0, 0.0, 0.0);
    float3 rayIllumination = float3(0.0, 0.0, 0.0);

#if (USE_SHADER_MODEL_6_5 == 1)

    if (!((normal.x == 0.0) && (normal.y == 0.0) && (normal.z == 0.0))) {

        uint rayFlags = RAY_FLAG_NONE;
#if USE_FIRST_HIT_END_SEARCH_SHADOWS
        // Shoot the ray in the reverse direction of the directional light
        rayDir = lightInCameraView;
        // Shoot the ray from the origin of the visible pixel position from the depth buffer
        origin = mul(float4(position.xyz, 1.0), inverseView).xyz;
        // Only need to figure out if one triangle blocks the directional light source
        rayFlags |= RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH;
#else
        // Why does the y component of the shadow texture mapping need to be 1.0 - yCoord?
        GenerateCameraRay(float2(shadowTextureCoordinates.x, 1.0 - shadowTextureCoordinates.y), origin, rayDir);
#endif

        // Trace the ray.
        // Set the ray's extents.
        RayDesc ray;
        ray.Origin    = origin;
        ray.Direction = rayDir;
        ray.TMin      = 0.1;
        ray.TMax      = MAX_DEPTH;

        RayQuery<RAY_FLAG_NONE> rayQuery;
        rayQuery.TraceRayInline(rtAS, rayFlags, ~0, ray);

        // Transparency testing
        while (rayQuery.Proceed() == true) {
            if (rayQuery.CandidateType() == CANDIDATE_NON_OPAQUE_TRIANGLE) {

                float  alphaValue   = 1.0f;
                float2 barycentrics = rayQuery.CandidateTriangleBarycentrics();

                if (rayQuery.CandidateInstanceID() == 1) {
                    float2 texCoord = GetTexCoord(rayQuery.CandidateTriangleBarycentrics(),
                                                  rayQuery.CandidateInstanceID(),
                                                  rayQuery.CandidatePrimitiveIndex());
                    alphaValue      = transparencyTexture1.Sample(textureSampler, texCoord).a;
                }

                if (alphaValue > 0.1) {
                    rayQuery.CommitNonOpaqueTriangleHit();
                }
            }
        }

        if (rayQuery.CommittedStatus() == COMMITTED_TRIANGLE_HIT) {
#if USE_FIRST_HIT_END_SEARCH_SHADOWS
            rtDepth = 0.0;
#else
            float3 hitPosition = rayQuery.WorldRayOrigin() + (rayQuery.CommittedRayT() * rayQuery.WorldRayDirection());

            float4x4 lightViewProj = mul(lightViewMatrix, lightProjectionMatrix);
            float4   clipSpace     = mul(float4(hitPosition, 1), lightViewProj);
            rtDepth                = clipSpace.z;
#endif
        }

        // AMBIENT OCCLUSION WIP
        //// Shoot short rays around hemisphere of the pixel's normal
        //const uint ambientOcclusionRayKernelSize = 2;
        //// Always same origin and ray length
        //ray.Origin = origin;
        //ray.TMax   = 25;
        //// Zero out ray flags and occlude transparent geometry and try first hit end search
        //rayFlags   = RAY_FLAG_CULL_NON_OPAQUE | RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH;

        //const float degreeOffset         = 180.0f       / ambientOcclusionRayKernelSize;
        //const float degreeStart          = degreeOffset / 2.0f;
        //const float occlusionDegradation = 1.0f         / (ambientOcclusionRayKernelSize * 2.0f);
        ////for (int x = 0; x < ambientOcclusionRayKernelSize; x++) {
        ////    for (int y = 0; y < ambientOcclusionRayKernelSize; y++) {
        ////        
        ////        float  xRot         = radians(degreeStart + (x * degreeOffset));
        ////        float  yRot         = radians(degreeStart + (y * degreeOffset));
        ////        ray.Direction       = float3(normalizedNormal.x * cos(xRot),
        ////                                     normalizedNormal.y * sin(yRot),
        ////                                     normalizedNormal.z * tan(yRot/xRot));
        //        ray.Direction       = float3(-normalizedNormal.x,
        //                                     -normalizedNormal.y,
        //                                     -normalizedNormal.z);
        //        RayQuery<RAY_FLAG_NONE> ambientOcclusionQuery;
        //        ambientOcclusionQuery.TraceRayInline(rtAS, rayFlags, ~0, ray);

        //        // Test for collisions and add occlusion if there is at least one hit
        //        ambientOcclusionQuery.Proceed();

        //        if (ambientOcclusionQuery.CommittedStatus() == COMMITTED_TRIANGLE_HIT) {
        //            //rtOcclusion -= occlusionDegradation;
        //            rtOcclusion = ambientOcclusionQuery.CommittedRayT() / ray.TMax;
        //        }
        ////    }
        ////}

        // Reflection single bounce WIP
        // Shoot the ray from the origin of the visible pixel position from the depth buffer

        float4 normalWorldSpace = mul(float4(normalizedNormal.xyz, 0.0), inverseView);
        if (normalWorldSpace.x == 0.0 && normalWorldSpace.y == 1.0 && normalWorldSpace.z == 0.0) {
        
            ray.Origin       = mul(float4(position.xyz, 1.0), inverseView).xyz;
            // Reflected ray from directional light hitting the surface material
            float3 cameraPos = float3(inverseView[3][0], inverseView[3][1], inverseView[3][2]);
            
            float3 cameraDirection = normalize(ray.Origin - cameraPos);
            ray.Direction          = cameraDirection - (2.0f * dot(cameraDirection, normalWorldSpace.xyz) * normalWorldSpace.xyz);
            ray.TMin               = 0.1;
            ray.TMax               = MAX_DEPTH;

            RayQuery<RAY_FLAG_NONE> reflectionRayQuery;
            reflectionRayQuery.TraceRayInline(rtAS, RAY_FLAG_NONE, ~0, ray);

             //Transparency reflections
            while (reflectionRayQuery.Proceed() == true) {
                if (reflectionRayQuery.CandidateType() == CANDIDATE_NON_OPAQUE_TRIANGLE) {
                    float  alphaValue   = 1.0f;
                    float2 barycentrics = reflectionRayQuery.CandidateTriangleBarycentrics();

                    if (reflectionRayQuery.CandidateInstanceID() == 1) {
                        float2 texCoord = GetTexCoord(reflectionRayQuery.CandidateTriangleBarycentrics(),
                                                      reflectionRayQuery.CandidateInstanceID(),
                                                      reflectionRayQuery.CandidatePrimitiveIndex());
                        alphaValue      = transparencyTexture1.Sample(textureSampler, texCoord).a;
                    }

                    if (alphaValue > 0.1) {
                        reflectionRayQuery.CommitNonOpaqueTriangleHit();
                    }
                }
            }

            if (reflectionRayQuery.CommittedStatus() == COMMITTED_TRIANGLE_HIT) {

                if (reflectionRayQuery.CommittedInstanceID() == 1) {
                    float2 texCoord = GetTexCoord(reflectionRayQuery.CommittedTriangleBarycentrics(),
                                                  reflectionRayQuery.CommittedInstanceID(),
                                                  reflectionRayQuery.CommittedPrimitiveIndex());
                    //rtReflection    = float3(texCoord.xy, 0.0);
                    rtReflection    = transparencyTexture1.Sample(textureSampler, texCoord).rgb;

                    // shadows for reflections uggghh gross
                    float3 hitPosition = reflectionRayQuery.WorldRayOrigin() +
                                         (reflectionRayQuery.CommittedRayT() * reflectionRayQuery.WorldRayDirection());

                    float4   clipSpace     = mul(float4(hitPosition, 1), viewProjection);
                    rtDepth                = clipSpace.z;

                    float3 worldSpaceNormal =
                        normalize(GetNormal(reflectionRayQuery.CommittedPrimitiveIndex()));

                    float3 viewSpaceNormal   =
                        normalize(float3(mul(float4(worldSpaceNormal, 0.0), normalMatrix).xyz));

                    rayIllumination = worldSpaceNormal;
                    //dot(lightInCameraView, viewSpaceNormal);
                }
            }
        }
    }
#endif

    if (views == 0) {
        // Detects if there is no screen space information and then displays skybox!
        if (normal.x == 0.0 && normal.y == 0.0 && normal.z == 0.0) {
            float4 dayColor   = skyboxDayTexture.Sample(textureSampler, float3(position.x, -position.y, position.z));
            float4 nightColor = skyboxNightTexture.Sample(textureSampler, float3(position.x, -position.y, position.z));
            pixel.color =
                (((1.0 - normalizedLight.y) / 2.0) * dayColor) + (((1.0 + normalizedLight.y) / 2.0) * nightColor);
            // skybox depth trick to have it displayed at the depth boundary
            // precision matters here and must be as close as possible to 1.0
            // the number of 9s can only go to 7 but no less than 4
            pixel.depth = 0.9999999;
        } else {
            float3 pointLighting = float3(0.0, 0.0, 0.0);
            float  totalShadow   = 1.0;
            float  pointShadow   = 1.0;

#if (USE_SHADER_MODEL_6_5 == 1)
            float d = rtDepth;
#else
            float d = cameraDepthTexture.Sample(textureSampler, invertedYCoord).r;
#endif
            // illumination is from directional light but we don't want to illuminate when the sun is past the horizon
            // aka night time
            if (normalizedLight.y <= 0.0) {
                // Only shadow in textures space
                if (shadowTextureCoordinates.x <= 1.0 && shadowTextureCoordinates.x >= 0.0 &&
                    shadowTextureCoordinates.y <= 1.0 && shadowTextureCoordinates.y >= 0.0) {

                    if (d < shadowMapping.z - bias) {
                        directionalShadow = shadowEffect;
                    }
                }
            } else {
                illumination = 0.0;
            }

            // Point lights always emit light versus directional sun shadows
            float numLights             = numPointLights;
            float totalPointLightEffect = 0.0;
            for (int i = 0; i < numPointLights; i++) {
                float3 pointLightDir     = position.xyz - pointLightPositions[i].xyz;
                float  distanceFromLight = length(pointLightDir);
                float  bias              = 0.1;
                if (distanceFromLight < pointLightRanges[i]) {
                    float3 pointLightDirNorm = normalize(-pointLightDir);
                    pointLighting += (dot(pointLightDirNorm, normalizedNormal)) *
                                     (1.0 - (distanceFromLight / (pointLightRanges[i]))) * pointLightColors[i];
                    totalPointLightEffect +=
                        dot(pointLightDirNorm, normalizedNormal) * (1.0 - (distanceFromLight / (pointLightRanges[i])));

                    float3 cubeMapTexCoords = mul(float4(position.xyz, 1.0), viewToModelMatrix).xyz -
                                              mul(float4(pointLightPositions[i].xyz, 1.0), viewToModelMatrix).xyz;
                    float distance = length(cubeMapTexCoords);
                    float cubeDepth =
                        depthMap.Sample(textureSampler, normalize(cubeMapTexCoords.xyz)).x * pointLightRanges[i];

                    if (cubeDepth + bias < distance) {
                        pointShadow -= (1.0 - shadowEffect);
                    }
                }
            }

            totalShadow = min(directionalShadow, pointShadow);
            float3 lightComponentIllumination =
                (illumination * diffuse.rgb * directionalShadow) + (pointLighting * diffuse.rgb * pointShadow);
            pixel.color = float4((illumination * diffuse.rgb * directionalShadow), 1.0);
        }
    } else if (views == 1) {
        pixel.color = float4(diffuse.rgb, 1.0);
    } else if (views == 2) {
        pixel.color = float4(normalizedNormal.xyz, 1.0);
    } else if (views == 3) {
        pixel.color = float4(normalize(position.xyz), 1.0);
    } else if (views == 4) {
        pixel.color = float4(float2(abs(velocity.r), abs(velocity.g)), 0.0, 1.0);
    } else if (views == 5) {
        pixel.color = float4(occlusion, occlusion, occlusion, 1.0);
        // pixel.color = float4(rtOcclusion, rtOcclusion, rtOcclusion, 1.0);
    } else if (views == 6) {
        float depth = cameraDepthTexture.Sample(textureSampler, uv).x;
        //pixel.color = float4(depth, depth, depth, 1.0);
        //pixel.depth = 0.1;
        //pixel.color = float4(rtOcclusion + (depth * 0.00001),
        //                     rtOcclusion + (depth * 0.00001),
        //                     rtOcclusion + (depth * 0.00001),
        //                     1.0);
        //pixel.color = float4(rtReflection.x + (depth * 0.00001),
        //                     rtReflection.y + (depth * 0.00001),
        //                     rtReflection.z + (depth * 0.00001),
        //                     1.0);
        pixel.color = float4(rayIllumination.x + (depth * 0.00001),
                             rayIllumination.y + (depth * 0.00001),
                             rayIllumination.z + (depth * 0.00001),
                             1.0);
    } else if (views == 7) {
        float2 screenPos = (2.0f * uv) - 1.0f;
        // Invert Y for DirectX-style coordinates.
        screenPos.y = -screenPos.y;
        // pixel.color = float4(screenPos.x, screenPos.y, 0.0, 1.0);

        float depth = rtDepth;
        pixel.color = float4(depth, depth, depth, 1.0);
        pixel.depth = 0.1;

        pixel.debug0 = float4(float3(origin.xyz), 1.0);
        pixel.debug1 = float4(float3(rayDir.xyz), 1.0);
    } else if (views == 8) {
        float3 cubeMapTexCoords = mul(float4(position.xyz, 1.0), viewToModelMatrix).xyz -
                                  mul(float4(pointLightPositions[0].xyz, 1.0), viewToModelMatrix).xyz;
        float cubeDepth = depthMap.Sample(textureSampler, normalize(cubeMapTexCoords.xyz)).x;
        pixel.color     = float4(cubeDepth, cubeDepth, cubeDepth, 1.0);
        pixel.depth     = 0.1;
    } else if (views == 9) {
        float depth = mapDepthTexture.Sample(textureSampler, uv).x;
        pixel.color = float4(depth, depth, depth, 1.0);
        pixel.depth = 0.1;
    }

    if (!(rtReflection.x == 0.0f && rtReflection.y == 0.0f && rtReflection.z == 0.0f) &&
        views == 0) {
        
        float rtDirectionalShadow = 1.0;
        //if (rtDepth < shadowMapping.z - bias) {
        //    rtDirectionalShadow = shadowEffect;
        //}
        pixel.color = float4((rayIllumination * rtReflection.rgb * rtDirectionalShadow), 1.0);
        //pixel.color = float4(rtReflection.rgb, 1.0);
    }

    return pixel;
}