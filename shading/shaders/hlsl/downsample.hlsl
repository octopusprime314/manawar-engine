
// Object Declarations
Texture2D          readTexture  : register(t0);
RWTexture2D<float> writeTexture : register(u0);

[numthreads(16, 16, 1)]
void CS(int3 threadId : SV_DispatchThreadID) {

    float result   = 0.0;
    //4x4 blocks downsampling
    int sampleRate = 4;
    for (int x = 0; x < sampleRate; x++)
    {
        for (int y = 0; y < sampleRate; y++)
        {
            result += readTexture.Load(int3((threadId.x * sampleRate) + x,
                                            (threadId.y * sampleRate) + y,
                                            0)).r;
        }
    }
    writeTexture[threadId.xy] = result / 16.0;
}
