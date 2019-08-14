
// Object Declarations
Texture2D readTexture            : register(t0);
RWTexture2D<float4> writeTexture : register(u0);

[numthreads(16, 16, 1)]
void CS(int3 DTid : SV_DispatchThreadID) {
    
    //upsample by 4x4 kernels
    int2 ID = int2(DTid.xy);
    float3 result = readTexture.Load(uint3(ID, 0)).rgb + writeTexture.Load(ID.xy).rgb;
    writeTexture[ID] = float4(result, 1.0);
}