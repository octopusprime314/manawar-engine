
// Object Declarations
Texture2D readTexture            : register(t0);
RWTexture2D<float> writeTexture    : register(u0);

[numthreads(16, 16, 1)]
void CS(int3 DTid : SV_DispatchThreadID) {
    
    int2 ID = int2(DTid.xy);
    float result = 0.0;
    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            int2 offset = int2(x, y);
            result += readTexture.Load(int3(ID + offset, 0)).r;
        }
    }
    writeTexture[ID] = result / 16.0;
}