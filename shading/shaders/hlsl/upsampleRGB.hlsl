// Object Declarations
Texture2D           readTexture  : register(t0);
RWTexture2D<float4> writeTexture : register(u0);

[numthreads(16, 16, 1)]
void CS(int3 threadId : SV_DispatchThreadID) {

    //4x4 blocks upsampling
    uint sampleRate           = 4;
    float3 result             = readTexture.Load(uint3((threadId.xy / sampleRate), 0)).rgb;
    writeTexture[threadId.xy] = float4(result, 1.0);
}
