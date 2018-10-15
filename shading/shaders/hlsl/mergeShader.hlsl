// Object Declarations
Texture2D deferredTexture   : register(t0);            // Color texture for mesh
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

    float4 texColor = float4(deferredTexture.Sample(textureSampler, uv).rgb, 1.0);
    return texColor;
}