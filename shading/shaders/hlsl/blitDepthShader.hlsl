
Texture2D inDepthTexture : register(t0);
sampler textureSampler : register(s0);

void VS(    uint   id          : SV_VERTEXID,
        out float4 outPosition : SV_POSITION,
        out float2 outUV       : UVOUT) {

    outPosition.x = (float)(id / 2) * 4.0 - 1.0;
    outPosition.y = (float)(id % 2) * 4.0 - 1.0;
    outPosition.z = 0.0;
    outPosition.w = 1.0;
    outUV.x       = (float)(id / 2) * 2.0;
    outUV.y       = 1.0 - (float)(id % 2) * 2.0;
}


struct PixelOut
{
    float depth : SV_Depth;
};

PixelOut PS(float4 position : SV_POSITION,
            float2 uv       : UVOUT)
{
    PixelOut pixel = { inDepthTexture.Sample(textureSampler, uv).r };
    return pixel;
}