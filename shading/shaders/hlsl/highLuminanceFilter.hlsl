// Object Declarations
Texture2D readTexture            : register(t0);
RWTexture2D<float4> writeTexture : register(u0);

[numthreads(16, 16, 1)]
void CS(int3 threadId : SV_DispatchThreadID) {

    float3 color       = readTexture.Load(int3(threadId.x, threadId.y, 0)).rgb;
    // check whether fragment output is higher than threshold, if so output as brightness color
    float brightness   = dot(color.rgb, float3(0.2126,
                                               0.7152,
                                               0.0722));
    float3 brightColor = float3(0.0, 0.0, 0.0);
    if(brightness >= 0.67){
        brightColor = float3(color.rgb);
    }
    writeTexture[threadId.xy] = float4(brightColor.rgb, 1.0);
}