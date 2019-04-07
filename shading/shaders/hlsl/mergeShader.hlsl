// Object Declarations
Texture2D deferredTexture   : register(t0);            // Color texture for mesh
Texture2D velocityTexture   : register(t1);            // Color texture for mesh
sampler   textureSampler    : register(s0);


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

float4 PS(float4 posH : SV_POSITION,
    float2 uv : UVOUT) : SV_Target {

    //Divide by 2 to prevent overblurring
    float2 velocityVector = velocityTexture.Sample(textureSampler, uv).xy / 8.0;

    float4 result = float4(0.0,0.0,0.0,0.0);
    float2 texCoords = uv;

    result += deferredTexture.Sample(textureSampler, texCoords) * 0.4;
    texCoords -= velocityVector;
    result += deferredTexture.Sample(textureSampler, texCoords) * 0.3;
    texCoords -= velocityVector;
    result += deferredTexture.Sample(textureSampler, texCoords) * 0.2;
    texCoords -= velocityVector;
    result += deferredTexture.Sample(textureSampler, texCoords) * 0.1;

    return float4(float3(result.rgb), 1.0);
}