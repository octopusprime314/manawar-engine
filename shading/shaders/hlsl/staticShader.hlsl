// Object Declarations
sampler   textureSampler : register(s0);
Texture2D textureMap     : register(t0);
Texture2D tex0           : register(t1);
Texture2D tex1           : register(t2);
Texture2D tex2           : register(t3);
Texture2D tex3           : register(t4);
Texture2D alphatex0      : register(t5);

cbuffer objectData : register(b0)
{
    float4x4 model;
    int      id;
    int      isLayeredTexture;
    int      primitiveOffset;
}
cbuffer globalData : register(b1)
{
    float4x4 prevModel;
    float4x4 prevView;
    float4x4 view;
    float4x4 projection;
    float4x4 normal;
}

void VS(in  float3 inPosition  : POSITION,
        in  float3 inNormal    : NORMAL,
        in  float2 inUV        : UV,
        out float4 outPosition : SV_POSITION,
        out float3 outNormal   : NORMALOUT,
        out float2 outUV       : UVOUT) {

    float4x4 mv  = mul(model, view);
    float4x4 mvp = mul(mv,    projection);
    outPosition  = mul(float4(inPosition, 1.0f), mvp);
    outNormal    = mul(float4(inNormal, 1.0f), normal).rgb;
    outUV        = float2(inUV.x, 1.0f - inUV.y);

}

struct MRT {
    float4 color  : SV_Target0;
    float4 normal : SV_Target1;
    float4 id     : SV_Target2;
};

MRT PS(in float4 position    : SV_POSITION,
       in float3 normal      : NORMALOUT,
       in float2 uv          : UVOUT,
       in uint   primitiveID : SV_PrimitiveID) {

    float4 vTexColor = float4(0.0, 0.0, 0.0, 1.0);
    MRT output;

    if (isLayeredTexture) {
        float4 red       = float4(tex0.Sample(textureSampler, mul(uv, 150.0)).rgb, 1.0);
        float4 green     = float4(tex1.Sample(textureSampler, mul(uv, 150.0)).rgb, 1.0);
        float4 blue      = float4(tex2.Sample(textureSampler, mul(uv, 150.0)).rgb, 1.0);
        float4 alpha     = float4(tex3.Sample(textureSampler, mul(uv, 150.0)).rgb, 1.0);
        float4 idTexture = float4(alphatex0.Sample(textureSampler, uv));

        //Used to prevent tiling
        float4 tRandoR   = float4(tex0.Sample(textureSampler, mul(uv, 5.0)).rgb, 1.0);
        float4 tRandoG   = float4(tex1.Sample(textureSampler, mul(uv, 5.0)).rgb, 1.0);
        float4 tRandoB   = float4(tex2.Sample(textureSampler, mul(uv, 5.0)).rgb, 1.0);
        float4 tRandoA   = float4(tex3.Sample(textureSampler, mul(uv, 5.0)).rgb, 1.0);

        float4 tR        = red * tRandoR;
        float4 tG        = green * tRandoG;
        float4 tB        = blue * tRandoB;
        float4 tA        = alpha * tRandoA;

        float redPixel   = idTexture.r;
        float greenPixel = idTexture.g;
        float bluePixel  = idTexture.b;
        //alpha value is inverse 
        float alphaPixel = 1.0 - idTexture.a;

        float4 lerpComponent = float4(0.0, 0.0, 0.0, 1.0);
        lerpComponent        = mul(redPixel,   tR) + mul((1.0f - redPixel),   lerpComponent);
        lerpComponent        = mul(greenPixel, tG) + mul((1.0f - greenPixel), lerpComponent);
        lerpComponent        = mul(bluePixel,  tB) + mul((1.0f - bluePixel),  lerpComponent);
        lerpComponent        = mul(alphaPixel, tA) + mul((1.0f - alphaPixel), lerpComponent);
        vTexColor            = float4(lerpComponent.rgb, 1.0);
        output.color         = vTexColor;
    }
    else {
        output.color = float4(textureMap.Sample(textureSampler, uv).rgb, 1.0);
    }
    output.normal = float4(normal.rgb,
                           1.0);
    output.id     = float4(0.0,
                           0.0,
                           float(id) / 16777216.0,
                           float(primitiveID + primitiveOffset) / 16777216.0);
    return output;
}