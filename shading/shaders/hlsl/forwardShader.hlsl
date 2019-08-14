// SRV data
Texture2D    textureMap         : register(t0);
Texture2D    cameraDepthTexture : register(t1);
Texture2D    mapDepthTexture    : register(t2);
TextureCube  depthMap           : register(t3);
sampler      textureSampler     : register(s0);
// Root constant data
cbuffer      objectData         : register(b0)
{
    float4x4 model;
}
// Constant Buffer data
cbuffer      globalData         : register(b1)
{
    //float4x4 prevModel;             // Previous Model transformation matrix
    //float4x4 prevView;              // Previous View/Camera transformation matrix
    float4x4 view;                    // View/Camera transformation matrix
    float4x4 projection;              // Projection transformation matrix
    float4x4 normalMatrix;            // Normal matrix
                                      
    float4x4 lightViewMatrix;         // Light perspective's view matrix
    float4x4 lightMapViewMatrix;      // Light perspective's view matrix
    float4x4 viewToModelMatrix;       // Inverse camera view space matrix

    float3   pointLightPositions[20]; // max lights is 20 for now
    float3   pointLightColors[20];    // max lights is 20 for now
    float    pointLightRanges[20];    // max lights is 20 for now
    int      numPointLights;
    int      views;                   // views set to 0 is diffuse mapping,
                                      // set to 1 is shadow mapping 
                                      // and set to 2 is normal mapping
    float3   lightDirection;
}

static const float2 poissonDisk[4] = {
    float2(-0.94201624,  -0.39906216),
    float2( 0.94558609,  -0.76890725),
    float2(-0.094184101, -0.92938870),
    float2( 0.34495938,   0.29387760)
};

static const float pointLightShadowEffect = 0.2;
static const float shadowEffect           = 0.6;
static const float ambient                = 0.3;

void VS(    float3 iPosL   : POSITION,
            float3 iNormal : NORMAL,
            float2 iUV     : UV,
        out float4 oPosH   : SV_POSITION,
        out float3 oNormal : NORMALOUT,
        out float2 oUV     : UVOUT) {

    float4x4 mv  = mul(model, view);
    float4x4 mvp = mul(mv, projection);
    oPosH        = mul(float4(iPosL, 1.0f), mvp);
    oUV          = iUV;
    oNormal      = mul(float4(iNormal, 1.0f), normalMatrix).rgb;
}

struct PixelOut
{
            float4 color  : SV_Target;
};

PixelOut PS(float4 posH   : SV_POSITION,
            float3 normal : NORMALOUT,
            float2 uv     : UVOUT) {

    PixelOut pixel;

    float4 diffuse = textureMap.Sample(textureSampler, uv);
    //If any transparency just discard this fragment
    if (diffuse.a < 0.1) {
        discard;
        pixel.color = float4(0.0, 0.0, 0.0, 0.0);
    }
    else {
        //Directional light calculation
        float3 normalizedLight   = normalize(lightDirection);
        float3 lightInCameraView = normalize(float3(mul(float4(-normalizedLight, 0.0), normalMatrix).xyz));
        float  illumination      = dot(lightInCameraView, normalize(normal));

        //Convert from camera space vertex to light clip space vertex
        float4 shadowMapping               = mul(float4(posH.xyz, 1.0), lightViewMatrix);
        shadowMapping                      = shadowMapping / shadowMapping.w;
        float2 shadowTextureCoordinates    = mul(shadowMapping.xy, 0.5) + float2(0.5, 0.5);

        float4 shadowMappingMap            = mul(float4(posH.xyz, 1.0), lightMapViewMatrix);
        shadowMappingMap                   = shadowMappingMap / shadowMappingMap.w;
        float2 shadowTextureCoordinatesMap = mul(shadowMappingMap.xy, 0.5) + float2(0.5, 0.5);

        float3 pointLighting               = float3(0.0, 0.0, 0.0);
        float  totalShadow                 = 1.0;
        float  directionalShadow           = 1.0;
        float  pointShadow                 = 1.0;
        //TODO: need to fix cpu
        float2 invertedYCoord    = float2( shadowTextureCoordinates.x,
                                          -shadowTextureCoordinates.y);
        float d                  = cameraDepthTexture.Sample(textureSampler, invertedYCoord).r;

        //illumination is from directional light but we don't want to illuminate when the sun is past the horizon
        //aka night time
        if (normalizedLight.y <= 0.0) {

            //removes shadow acne by adding a small bias
            const float bias = 0.005;
            //Only shadow in textures space
            if (shadowTextureCoordinates.x <= 1.0 &&
                shadowTextureCoordinates.x >= 0.0 &&
                shadowTextureCoordinates.y <= 1.0 &&
                shadowTextureCoordinates.y >= 0.0) {

                if (d < shadowMapping.z - bias) {
                    directionalShadow = shadowEffect;
                }
            }
            else if (shadowTextureCoordinatesMap.x <= 1.0 &&
                     shadowTextureCoordinatesMap.x >= 0.0 &&
                     shadowTextureCoordinatesMap.y <= 1.0 &&
                     shadowTextureCoordinatesMap.y >= 0.0) {

                //TODO: need to fix cpu
                float2 invertedYCoord = float2( shadowTextureCoordinatesMap.x,
                                               -shadowTextureCoordinatesMap.y);
                if (mapDepthTexture.Sample(textureSampler, invertedYCoord).r < shadowMappingMap.z - bias) {
                    directionalShadow = shadowEffect;
                }
            }
        }
        else {
            illumination = 0.0;
        }

        //Point lights always emit light versus directional sun shadows
        float numLights                  = numPointLights;
        float totalPointLightEffect      = 0.0;
        for (int i = 0; i < numPointLights; i++) {
            float3 pointLightDir         = posH.xyz - pointLightPositions[i].xyz;
            float  distanceFromLight     = length(pointLightDir);
            float  bias                  = 0.1;
            if (distanceFromLight < pointLightRanges[i]) {
                float3 pointLightDirNorm = normalize(-pointLightDir);
                pointLighting           += (dot(pointLightDirNorm, normal)) *
                                           (1.0 - (distanceFromLight / (pointLightRanges[i]))) *
                                            pointLightColors[i];
                totalPointLightEffect   += dot(pointLightDirNorm, normal) *
                                           (1.0 - (distanceFromLight / (pointLightRanges[i])));

                float3 cubeMapTexCoords  = mul(float4(posH.xyz, 1.0), viewToModelMatrix).xyz -
                                           mul(float4(pointLightPositions[i].xyz, 1.0), viewToModelMatrix).xyz;
                float distance           = length(cubeMapTexCoords);
                float cubeDepth          = depthMap.Sample(textureSampler, 
                                                           normalize(cubeMapTexCoords.xyz)).x*pointLightRanges[i];

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

    return pixel;
}