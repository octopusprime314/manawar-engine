
// Object Declarations
Texture2D readTexture            : register(t0);
RWTexture2D<float4> writeTexture : register(u0);

[numthreads(16, 16, 1)]
void CS(int3 DTid : SV_DispatchThreadID) {

    int2 ID = int2(DTid.xy);
    float3 result = float3(0.0, 0.0, 0.0);

    //4x4 blocks downsampling
    int sampleRate = 4;
    for (int x = 0; x < sampleRate; x++)
    {
        for (int y = 0; y < sampleRate; y++)
        {
            result += readTexture.Load(int3((ID.x * sampleRate) + x, (ID.y * sampleRate) + y, 0)).rgb;
        }
    }
    writeTexture[ID] = float4(float3(result / 16.0), 1.0);
}
