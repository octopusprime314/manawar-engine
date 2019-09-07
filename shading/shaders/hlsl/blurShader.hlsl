// Object Declarations
Texture2D          readTexture  : register(t0);
RWTexture2D<float> writeTexture : register(u0);

[numthreads(16, 16, 1)]
void CS(int3 threadId : SV_DispatchThreadID) {

    float result = 0.0;
    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            int2 offset = int2(x, y);
            result     += readTexture.Load(int3(threadId.xy + offset, 0)).r;
        }
    }
    writeTexture[threadId.xy] = result / 16.0;
}