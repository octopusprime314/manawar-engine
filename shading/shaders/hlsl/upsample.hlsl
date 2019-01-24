
// Object Declarations
Texture2D readTexture            : register(t0);
RWTexture2D<float> writeTexture    : register(u0);

[numthreads(16, 16, 1)]
void CS(int3 DTid : SV_DispatchThreadID) {
    
    uint2 ID = uint2(DTid.xy);

    //4x4 blocks upsampling
    uint sampleRate = 4;
    float result = readTexture.Load(uint3((ID / sampleRate), 0)).r;

    writeTexture[ID] = result;
}
