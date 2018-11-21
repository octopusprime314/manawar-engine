
// Object Declarations
Texture2D readTexture            : register(t0);
RWTexture2D<float> writeTexture    : register(u0);

[numthreads(16, 16, 1)]
void CS(int3 DTid : SV_DispatchThreadID) {
    
    int2 ID = int2(DTid.xy);

    //4x4 blocks upsampling
    int sampleRate = 4;
    float result = readTexture.Load(int3((ID / sampleRate), 0)).r;

    writeTexture[ID] = result;
}
