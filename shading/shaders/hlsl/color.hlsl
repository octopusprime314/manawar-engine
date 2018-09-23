// Object Declarations
Texture2D textureMap : register(t0);            // Color texture for mesh
sampler textureSampler : register(s0);


cbuffer mvp : register(b0)
{
	float4x4 gWorldViewProj;
}
void VS(float3 iPosL : POSITION,
	    float2 iUV : UV,
		out float4 oPosH : SV_POSITION,
		out float2 oUV : UVOUT) {
		
	oPosH = mul(float4(iPosL, 1.0f), gWorldViewProj);
	oUV = iUV;
}

float4 PS(float4 posH : SV_POSITION,
		  float2 uv : UVOUT) : SV_Target {
	return float4(textureMap.Sample(textureSampler, uv).rgb, 1.0);
}