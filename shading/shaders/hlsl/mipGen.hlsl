
cbuffer objectData : register(b0)
{
    float2 texelSize;   // 1.0 / destination dimension
}

Texture2D<float4>   srcTexture    : register(t0);
RWTexture2D<float4> dstTexture    : register(u0);
SamplerState        bilinearClamp : register(s0);

[numthreads(8, 8, 1)]
void CS(int3 DTid : SV_DispatchThreadID)
{
    //DTid is the thread ID * the values from numthreads above and in this case correspond to the pixels location in number of pixels.
    //As a result texcoords (in 0-1 range) will point at the center between the 4 pixels used for the mipmap.
    float2 texcoords = texelSize * (DTid.xy + 0.5);

    //The samplers linear interpolation will mix the four pixel values to the new pixels color
    float4 color = srcTexture.SampleLevel(bilinearClamp, 
        texcoords, 0);

    //Write the final color into the destination texture.
    dstTexture[DTid.xy] = color;
}
