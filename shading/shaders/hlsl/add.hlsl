// Object Declarations
Texture2D           readTexture  : register(t0);
RWTexture2D<float4> writeTexture : register(u0);

[numthreads(16, 16, 1)]
void CS(int3 threadId : SV_DispatchThreadID) {

    //upsample by 4x4 kernels
    float3 result             = readTexture.Load(uint3(threadId.xy, 0)).rgb +
                                writeTexture.Load(threadId.xy).rgb;
    writeTexture[threadId.xy] = float4(result, 1.0);
}