// Object Declarations

cbuffer objectData : register(b0)
{
    float4x4 model;
}
void VS(float3 iPosL : POSITION,
    out float4 oPosH : SV_POSITION) {

    oPosH = mul(float4(iPosL, 1.0f), model);
}