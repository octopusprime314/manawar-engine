Texture2D normalTexture : register(t0);
Texture2D noiseTexture : register(t1);
Texture2D depthTexture : register(t2);

sampler textureSampler : register(s0);

cbuffer globalData : register(b0) {
    float3 kernel[64];     	//Light perspective's view matrix
    float4x4 projection;    //Light perspective's view matrix
    float4x4 projectionToViewMatrix;		//Inverse camera view space matrix
}

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


// tile noise texture over screen based on screen dimensions divided by noise size
static const float2 noiseScale = float2(1920.0/4.0, 1080.0/4.0); // screen = 1280x720

float3 decodeLocation(float2 uv) {
    float4 clipSpaceLocation;
    clipSpaceLocation.xy = mul(uv, 2.0f) - 1.0f;
    clipSpaceLocation.y = -clipSpaceLocation.y; //TODO: need to fix cpu
    clipSpaceLocation.z = depthTexture.Sample(textureSampler, uv).r; //dx z clip space is [0,1]
    clipSpaceLocation.w = 1.0f;
    float4 homogenousLocation = mul(clipSpaceLocation, projectionToViewMatrix);
    return homogenousLocation.xyz / homogenousLocation.w;
}

struct PixelOut
{
    float4 color : SV_Target;
};

PixelOut PS(float4 posH : SV_POSITION,
    float2 uv : UVOUT)
{
    PixelOut pixel;

	int kernelSize = 64;
	float radius   = 5.0;
	float bias     = 0.05;
    float3 fragPos   = decodeLocation(uv);
	
	if(fragPos.x != 0.0 && fragPos.y != 0.0 && fragPos.z != 0.0){
		float3 normal    = normalTexture.Sample(textureSampler, uv).rgb;
        float3 randomVec = noiseTexture.Sample(textureSampler, uv * noiseScale).xyz;

        float3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
        float3 bitangent = cross(normal, tangent);
		float3x3 TBN     = float3x3(tangent, bitangent, normal); 

		float occlusion = 0.0;
		for(int i = 0; i < kernelSize; i = i + int(radius))
		{
			// get sample position
			float3 sampleValue = mul(kernel[i], TBN); // From tangent to view-space
            sampleValue = fragPos + sampleValue * radius;

            float4 offset = float4(sampleValue, 1.0);
			offset      = mul(offset, projection);    // from view to clip-space
			offset.xyz /= offset.w;               // perspective divide
			offset.xyz  = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0 
		
			float sampleDepth = decodeLocation(offset.xy).z;
			float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
			occlusion += (sampleDepth >= sampleValue.z + bias ? 1.0 : 0.0) * rangeCheck;
		}  
		occlusion = 1.0 - (occlusion / kernelSize);
        pixel.color.r = occlusion;
        return pixel;
	}
    else {
        return pixel;
    }
}