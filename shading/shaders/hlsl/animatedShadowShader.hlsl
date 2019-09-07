// Object Declarations
cbuffer objectData : register(b0)
{
    float4x4 model;
}
cbuffer globalData : register(b1)
{
    float4x4 view;
    float4x4 projection;
}

void VS(    float3 inPosition  : POSITION,
        out float4 outPosition : SV_POSITION) {

    float4x4 mv  = mul(model, view);
    float4x4 mvp = mul(mv, projection);
    outPosition  = mul(float4(inPosition, 1.0f), mvp);
}