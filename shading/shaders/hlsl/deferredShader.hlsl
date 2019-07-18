// Object Declarations

Texture2D diffuseTexture : register(t0);   //Diffuse texture data array
Texture2D normalTexture  : register(t1);    //Normal texture data array
Texture2D velocityTexture : register(t2); //velocity texture data array
Texture2D depthTexture : register(t3);    //depth texture data array
Texture2D cameraDepthTexture : register(t4);   //depth texture data array with values 1.0 to 0.0, with 0.0 being closer
Texture2D mapDepthTexture : register(t5);      //depth texture data array with values 1.0 to 0.0, with 0.0 being closer
TextureCube depthMap : register(t6);		//cube depth map for point light shadows
TextureCube skyboxDayTexture : register(t7);   //skybox day
TextureCube skyboxNightTexture : register(t8);	//skybox night
Texture2D ssaoTexture : register(t9);      //depth texture data array with values 1.0 to 0.0, with 0.0 being closer
               
sampler textureSampler : register(s0);

cbuffer globalData : register(b0) {
    float4x4 lightViewMatrix;     	//Light perspective's view matrix
    float4x4 lightProjectionMatrix;     	//Light perspective's view matrix
    float4x4 inverseView;     	//Light perspective's view matrix
    float4x4 lightMapViewMatrix;    //Light perspective's view matrix
    float4x4 viewToModelMatrix;		//Inverse camera view space matrix
    float4x4 projectionToViewMatrix; //Inverse projection matrix
    float4x4 normalMatrix;           //inverse transpose of view matrix

    float3 pointLightPositions[20];//max lights is 20 for now
    float3 pointLightColors[20]; //max lights is 20 for now
    float pointLightRanges[20];//max lights is 20 for now
    int  numPointLights;

    int views;   //views set to 0 is diffuse mapping, set to 1 is shadow mapping and set to 2 is normal mapping
    float3 lightDirection;
}

static const float2 poissonDisk[4] = {
    float2(-0.94201624, -0.39906216),
    float2(0.94558609, -0.76890725),
    float2(-0.094184101, -0.92938870),
    float2(0.34495938, 0.29387760)
};

static const float ambient = 0.3;
static const float shadowEffect = 0.6;

float3 decodeLocation(float2 uv) {
    float4 clipSpaceLocation;
    clipSpaceLocation.xy = mul(uv, 2.0f) - 1.0f;
    clipSpaceLocation.y = -clipSpaceLocation.y; //TODO: need to fix cpu
    clipSpaceLocation.z = depthTexture.Sample(textureSampler, uv).r; //dx z clip space is [0,1]
    clipSpaceLocation.w = 1.0f;
    float4 homogenousLocation = mul(clipSpaceLocation, projectionToViewMatrix);
    return homogenousLocation.xyz / homogenousLocation.w;
}

void VS(uint id : SV_VERTEXID,
    out float4 oPosH : SV_POSITION,
    out float2 oUV : UVOUT) {

    oPosH.x = (float)(id / 2) * 4.0 - 1.0;
    oPosH.y = (float)(id % 2) * 4.0 - 1.0;
    oPosH.z = 0.0;
    oPosH.w = 1.0;

    oUV.x =       (float)(id / 2) * 2.0;
    oUV.y = 1.0 - (float)(id % 2) * 2.0;
}
struct PixelOut
{
    float4 color : SV_Target;
    float depth : SV_Depth;
};

PixelOut PS(float4 posH : SV_POSITION,
    float2 uv : UVOUT) {

    PixelOut pixel = { float4(0.0, 0.0, 0.0, 0.0), 1.0 };

    //extract position from depth texture
    float4 position = float4(decodeLocation(uv), 1.0);
    //extract normal from normal texture
    float4 normal = normalTexture.Sample(textureSampler, uv);
    float3 normalizedNormal = normalize(normal.xyz);
    //extract color from diffuse texture
    float4 diffuse = diffuseTexture.Sample(textureSampler, uv);
    //extract 2d velocity buffer
    float2 velocity = velocityTexture.Sample(textureSampler, uv).rg;

    float occlusion = ssaoTexture.Sample(textureSampler, uv).r;

    //blit depth
    pixel.depth = depthTexture.Sample(textureSampler, uv).r;

    //Directional light calculation
    float3 normalizedLight   = normalize(lightDirection);
    float3 lightInCameraView = normalize(float3(mul(float4(-normalizedLight, 0.0), normalMatrix).xyz));
    float  illumination      = dot(lightInCameraView, normalizedNormal);

    float4x4 inverseToLightView = mul(inverseView, lightViewMatrix);
    float4x4 inverseLightViewProjection = mul(inverseToLightView, lightProjectionMatrix);

    //Convert from camera space vertex to light clip space vertex
    float4 shadowMapping = mul(float4(position.xyz, 1.0), inverseLightViewProjection);
    shadowMapping = shadowMapping / shadowMapping.w;
    float2 shadowTextureCoordinates = mul(shadowMapping.xy, 0.5) + float2(0.5, 0.5);

    float4 shadowMappingMap = mul(float4(position.xyz, 1.0), lightMapViewMatrix);
    shadowMappingMap = shadowMappingMap / shadowMappingMap.w;
    float2 shadowTextureCoordinatesMap = mul(shadowMappingMap.xy, 0.5) + float2(0.5,0.5);

    if (views == 0) {
        //Detects if there is no screen space information and then displays skybox!
        if (normal.x == 0.0 && normal.y == 0.0 && normal.z == 0.0) {
            float4 dayColor = skyboxDayTexture.Sample(textureSampler, float3(position.x, -position.y, position.z));
            float4 nightColor = skyboxNightTexture.Sample(textureSampler, float3(position.x, -position.y, position.z));
            pixel.color = (((1.0 - normalizedLight.y) / 2.0) * dayColor) + (((1.0 + normalizedLight.y) / 2.0) * nightColor);
            //skybox depth trick to have it displayed at the depth boundary
            //precision matters here and must be as close as possible to 1.0
            //the number of 9s can only go to 7 but no less than 4
            pixel.depth = 0.9999999;
        }
        else {
            float3 pointLighting = float3(0.0, 0.0, 0.0);
            float totalShadow = 1.0;
            float directionalShadow = 1.0;
            float pointShadow = 1.0;
            float2 invertedYCoord = float2(shadowTextureCoordinates.x, -shadowTextureCoordinates.y); //TODO: need to fix cpu
            float d = cameraDepthTexture.Sample(textureSampler, invertedYCoord).r;
            //illumination is from directional light but we don't want to illuminate when the sun is past the horizon
            //aka night time
            if (normalizedLight.y <= 0.0) {
                const float bias = 0.005; //removes shadow acne by adding a small bias
                //Only shadow in textures space
                if (shadowTextureCoordinates.x <= 1.0 && shadowTextureCoordinates.x >= 0.0 && shadowTextureCoordinates.y <= 1.0 && shadowTextureCoordinates.y >= 0.0) {

                    //Crap z is in [0,1] not in [-1,1]
                    if (d < shadowMapping.z - bias) {
                        directionalShadow = shadowEffect;
                    }
                }
                //else if (shadowTextureCoordinatesMap.x <= 1.0 && shadowTextureCoordinatesMap.x >= 0.0 && shadowTextureCoordinatesMap.y <= 1.0 && shadowTextureCoordinatesMap.y >= 0.0) {
                //    float2 invertedYCoord = float2(shadowTextureCoordinatesMap.x, -shadowTextureCoordinatesMap.y); //TODO: need to fix cpu
                //    if (mapDepthTexture.Sample(textureSampler, invertedYCoord).r < shadowMappingMap.z - bias) {
                //        directionalShadow = shadowEffect;
                //    }
                //}

            }
            else {
                illumination = 0.0;
            }

            //Point lights always emit light versus directional sun shadows
            float numLights = numPointLights;
            float totalPointLightEffect = 0.0;
            for (int i = 0; i < numPointLights; i++) {
                float3 pointLightDir = position.xyz - pointLightPositions[i].xyz;
                float distanceFromLight = length(pointLightDir);
                float bias = 0.1;
                if (distanceFromLight < pointLightRanges[i]) {
                    float3 pointLightDirNorm = normalize(-pointLightDir);
                    pointLighting += (dot(pointLightDirNorm, normalizedNormal)) * (1.0 - (distanceFromLight / (pointLightRanges[i]))) * pointLightColors[i];
                    totalPointLightEffect += dot(pointLightDirNorm, normalizedNormal) * (1.0 - (distanceFromLight / (pointLightRanges[i])));

                    float3 cubeMapTexCoords = mul(float4(position.xyz,1.0), viewToModelMatrix).xyz - mul(float4(pointLightPositions[i].xyz, 1.0), viewToModelMatrix).xyz;
                    float distance = length(cubeMapTexCoords);
                    float cubeDepth = depthMap.Sample(textureSampler, normalize(cubeMapTexCoords.xyz)).x*pointLightRanges[i];

                    if (cubeDepth + bias < distance) {
                        //pointShadow -= ((1.0 - pointLightShadowEffect)/numLights)*(1.0 - (distance/cubeDepth));
                        //pointShadow -= ((1.0 - shadowEffect)/numLights);
                        //USE ONLY ONE POINT SHADOW FOR NOW!!!!
                        pointShadow -= (1.0 - shadowEffect);
                    }
                }
            }

            totalShadow = min(directionalShadow, pointShadow);

            //If all light components add up to more than one, then normalize
            //light components other than ambient
            //if(illumination + totalPointLightEffect + ambient > 1.0){
            //	float2 lightNormalized = normalize(float2(illumination, totalPointLightEffect));
            //	illumination = lightNormalized.x - (ambient / 2);
            //	pointLighting = (pointLighting * lightNormalized.y) - (ambient / 2);
            //}

            float3 lightComponentIllumination = (illumination  * diffuse.rgb * directionalShadow) +
                (pointLighting * diffuse.rgb * pointShadow);

            //pixel.color = float4((lightComponentIllumination)+(ambient * diffuse.rgb * occlusion), 1.0);
            /*if (shadowTextureCoordinates.x <= 1.0 && shadowTextureCoordinates.x >= 0.0 && shadowTextureCoordinates.y <= 1.0 && shadowTextureCoordinates.y >= 0.0) {
                pixel.color = float4(d, d, d, 1.0);
            }
            else {
                pixel.color = float4(1.0, 1.0, 1.0, 1.0);
            }
            pixel.depth = shadowMapping.z;*/

            pixel.color = float4((illumination * diffuse.rgb * directionalShadow), 1.0);

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

        float depth = mapDepthTexture.Sample(textureSampler, uv).x;
        pixel.color = float4(depth, depth, depth, 1.0);
        pixel.depth = 0.1;
    }
    else if (views == 8) {

        float3 cubeMapTexCoords = mul(float4(position.xyz,1.0), viewToModelMatrix).xyz - mul(float4(pointLightPositions[0].xyz, 1.0), viewToModelMatrix).xyz;
        float cubeDepth = depthMap.Sample(textureSampler, normalize(cubeMapTexCoords.xyz)).x;
        pixel.color = float4(cubeDepth, cubeDepth, cubeDepth, 1.0);
        pixel.depth = 0.1;
    }
    else if (views == 9) {
        //Draw geometry visualizer
    }
    return pixel;
}