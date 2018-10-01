// Object Declarations
Texture2D textureMap   : register(t0);            // Color texture for mesh
Texture2D tex0         : register(t1);            // Color texture for mesh
Texture2D tex1         : register(t2);            // Color texture for mesh
Texture2D tex2         : register(t3);            // Color texture for mesh
Texture2D tex3         : register(t4);            // Color texture for mesh
Texture2D alphatex0    : register(t5);            // Color texture for mesh
sampler textureSampler : register(s0);


cbuffer model : register(b0)
{
    //float4x4 prevModel;  // Previous Model transformation matrix
    //float4x4 prevView;   // Previous View/Camera transformation matrix
    float4x4 modelMatrix;		 // Model and World transformation matrix
    int     isLayeredTexture;
    //float4x4 view;		 // View/Camera transformation matrix
    //float4x4 projection; // Projection transformation matrix
    //float4x4 normal;     // Normal matrix
}
cbuffer view : register(b1)
{
    float4x4 viewMatrix;		 // View/Camera transformation matrix
}
cbuffer projection : register(b2)
{
    float4x4 projectionMatrix; // Projection transformation matrix
}
void VS(float3 iPosL : POSITION,
	    float2 iUV : UV,
		out float4 oPosH : SV_POSITION,
		out float2 oUV : UVOUT) {
	
    float4x4 mv = mul(modelMatrix, viewMatrix);
    float4x4 mvp = mul(mv, projectionMatrix);
	oPosH = mul(float4(iPosL, 1.0f), mvp);
	oUV = iUV;
}

float4 PS(float4 posH : SV_POSITION,
		  float2 uv : UVOUT) : SV_Target {

    float4 vTexColor = float4(0.0, 0.0, 0.0, 1.0);

    if (modelMatrix[0][0] == 1.0/*isLayeredTexture == 0*/) {
        float4 red = float4(tex0.Sample(textureSampler, mul(uv,150.0)).rgb, 1.0);
        float4 green = float4(tex1.Sample(textureSampler, mul(uv, 150.0)).rgb, 1.0);
        float4 blue = float4(tex2.Sample(textureSampler, mul(uv, 150.0)).rgb, 1.0);
        float4 alpha = float4(tex3.Sample(textureSampler, mul(uv, 150.0)).rgb, 1.0);
        float4 idTexture = float4(alphatex0.Sample(textureSampler, uv));

        //Used to prevent tiling

        float4 tRandoR = float4(tex0.Sample(textureSampler, mul(uv, 5.0)));
        float4 tRandoG = float4(tex1.Sample(textureSampler, mul(uv, 5.0)));
        float4 tRandoB = float4(tex2.Sample(textureSampler, mul(uv, 5.0)));
        float4 tRandoA = float4(tex3.Sample(textureSampler, mul(uv, 5.0)));

        float4 tR = mul(red,tRandoR);
        float4 tG = mul(green,tRandoG);
        float4 tB = mul(blue,tRandoB);
        float4 tA = mul(alpha,tRandoA);

        float redPixel = idTexture.r;
        float greenPixel = idTexture.g;
        float bluePixel = idTexture.b;
        float alphaPixel = 1.0 - idTexture.a; //alpha value is inverse 

        float4 lerpComponent = float4(0.0, 0.0, 0.0, 1.0);
        //Path next
        lerpComponent = mul(redPixel, tR) + mul((1.0 - redPixel),lerpComponent);
        //Mountain next
        lerpComponent = mul(greenPixel, tG) + mul((1.0f - greenPixel),lerpComponent);
        //Dirt next
        lerpComponent = mul(bluePixel, tB) + mul((1.0f - bluePixel),lerpComponent);
        //snow next
        lerpComponent = mul(alphaPixel, tA) + mul((1.0f - alphaPixel),lerpComponent);

        vTexColor = float4(lerpComponent.rgb, 1.0);
        return vTexColor;
    }
    else {
        return float4(textureMap.Sample(textureSampler, uv).rgb, 1.0);
    }


	//return float4(textureMap.Sample(textureSampler, uv).rgb, 1.0);
}