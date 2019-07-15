// Object Declarations
cbuffer objectData : register(b0)
{
    float4x4 model;
}
cbuffer globalData : register(b1)
{
    float4x4 view;		 // View/Camera transformation matrix
    float4x4 projection; // Projection transformation matrix
}

void VS(float3 iPosL : POSITION,
    out float4 oPosH : SV_POSITION) {

    float4x4 mv = mul(model, view);
    float4x4 mvp = mul(mv, projection);
    oPosH = mul(float4(iPosL, 1.0f), mvp);
}