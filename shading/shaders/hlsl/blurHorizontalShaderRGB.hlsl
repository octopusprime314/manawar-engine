// Object Declarations
Texture2D readTexture            : register(t0);
RWTexture2D<float4> writeTexture : register(u0);

[numthreads(16, 16, 1)]
void CS(int3 DTid : SV_DispatchThreadID) {
    static float weight[5] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };

    int2 ID = int2(DTid.xy);
    float3 result = readTexture.Load(int3(ID.x, ID.y, 0)).rgb * weight[0]; // current fragment's contribution

    for (int i = 1; i < 5; ++i)
    {
        result += readTexture.Load(int3(ID.x + i, ID.y, 0)).rgb * weight[i];
        result += readTexture.Load(int3(ID.x - i, ID.y, 0)).rgb * weight[i];
    }
    writeTexture[ID] = float4(result.rgb, 1.0);
}