
Texture2D inDepthTexture : register(t0);
sampler textureSampler : register(s0);

void VS(uint id : SV_VERTEXID,
    out float4 oPosH : SV_POSITION,
    out float2 oUV : UVOUT) {

    oPosH.x = (float)(id / 2) * 4.0 - 1.0;
    oPosH.y = (float)(id % 2) * 4.0 - 1.0;
    oPosH.z = 0.0;
    oPosH.w = 1.0;

    oUV.x = (float)(id / 2) * 2.0;
    oUV.y = 1.0 - (float)(id % 2) * 2.0;
}


struct PixelOut
{
    float depth : SV_Depth;
};

PixelOut PS(float4 posH : SV_POSITION,
    float2 uv : UVOUT)
{
    PixelOut pixel;

    pixel.depth = inDepthTexture.Sample(textureSampler, uv).r;

    return pixel;
}