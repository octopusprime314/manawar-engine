// Object Declarations
Texture2D deferredTexture : register(t0);
Texture2D velocityTexture : register(t1);
sampler   textureSampler  : register(s0);

void VS(    uint   id          : SV_VERTEXID,
        out float4 outPosition : SV_POSITION,
        out float2 outUV       : UVOUT) {

    outPosition.x = (float)(id / 2) * 4.0 - 1.0;
    outPosition.y = (float)(id % 2) * 4.0 - 1.0;
    outPosition.z = 0.0;
    outPosition.w = 1.0;
    outUV.x       =       (float)(id / 2) * 2.0;
    outUV.y       = 1.0 - (float)(id % 2) * 2.0;
}

float4 PS(float4 posH : SV_POSITION,
          float2 uv   : UVOUT)
    : SV_Target {

    float4 result         = float4(0.0,0.0,0.0,0.0);
    //Divide by 2 to prevent overblurring
    float2 velocityVector = velocityTexture.Sample(textureSampler, uv).xy / 8.0;
    float2 texCoords      = uv;

    result    += deferredTexture.Sample(textureSampler, texCoords) * 0.4;
    texCoords -= velocityVector;
    result    += deferredTexture.Sample(textureSampler, texCoords) * 0.3;
    texCoords -= velocityVector;
    result    += deferredTexture.Sample(textureSampler, texCoords) * 0.2;
    texCoords -= velocityVector;
    result    += deferredTexture.Sample(textureSampler, texCoords) * 0.1;

    return float4(float3(result.rgb), 1.0);
}