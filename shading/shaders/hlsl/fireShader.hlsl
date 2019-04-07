
// Object Declarations
Texture2D readTexture            : register(t0);
RWTexture2D<float> writeTexture    : register(u0);

cbuffer objectData : register(b0) {
    float4x4 model;
    int      fireType;
    float    time;
    float3   fireColor;
}

cbuffer globalData : register(b1) {
    float4x4 view;              // View/Camera transformation matrix
    float4x4 projection;        // Projection transformation matrix
    float4x4 inverseViewNoTrans;// Inverse matrix
}

void VS(uint id : SV_VERTEXID,
        out float4 oPosH : SV_POSITION,
        out float2 oUV : UVOUT,
        out float3 oPos : POSOUT) {

    //there are five possible fire types in the pixel shader
    //choosing which fire type is in intervals of 0.2 in the u coordinate
    //and the v coordinates will always be between 0 and 0.5 for now
    float minU = fireType * 0.2f;
    float maxU = minU + 0.2f;
    float3 vertex = float3(0.0,0.0,0.0);

    if (id == 0) {
        vertex = float3(-1.0, 1.0, 0.0);
        oUV = float2(minU, 0.5);
    }
    else if (id == 1) {
        vertex = float3(-1.0, -1.0, 0.0);
        oUV = float2(minU, 0.0);
    }
    else if (id == 2) {
        vertex = float3(1.0, 1.0, 0.0);
        oUV = float2(maxU, 0.5);
    }
    else if (id == 3) {
        vertex = float3(1.0, -1.0, 0.0);
        oUV = float2(maxU, 0.0);
    }

    // The vertex is first transformed by the model and world, then 
    // the view/camera and finally the projection matrix
    // The order in which transformation matrices affect the vertex
    // is in the order from right to left
    float4x4 inverse = mul(inverseViewNoTrans, model);
    float4x4 imv = mul(inverse, view);
    float4x4 imvp = mul(imv, projection);
    float4 transformedVert = mul(float4(vertex.xyz, 1.0f), imvp);

    oPosH = transformedVert;

    oPos = mul(float4(vertex.xyz, 1.0), imv).xyz;

}


// procedural noise from IQ
float2 hash(float2 p) {
    p = float2(dot(p, float2(127.1, 311.7)),
        dot(p, float2(269.5, 183.3)));
    return -1.0 + 2.0*frac(sin(p)*43758.5453123);
}

float noise(in float2 p) {
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;

    float2 i = floor(p + (p.x + p.y)*K1);

    float2 a = p - i + (i.x + i.y)*K2;
    float2 o = (a.x > a.y) ? float2(1.0, 0.0) : float2(0.0, 1.0);
    float2 b = a - o + K2;
    float2 c = a - 1.0 + 2.0*K2;

    float3 h = max(0.5 - float3(dot(a, a), dot(b, b), dot(c, c)), 0.0);

    float3 n = h * h*h*h*float3(dot(a, hash(i + 0.0)), dot(b, hash(i + o)), dot(c, hash(i + 1.0)));

    return dot(n, float3(70.0, 70.0, 70.0));
}

float fbm(float2 uv) {
    float f;
    float2x2 m = float2x2(1.6, 1.2, -1.2, 1.6);
    f = 0.5000*noise(uv); 
    uv = mul(uv, m);
    f += 0.2500*noise(uv); 
    uv = mul(uv, m);
    f += 0.1250*noise(uv); 
    uv = mul(uv, m);
    f += 0.0625*noise(uv); 
    uv = mul(uv, m);
    f = 0.5 + 0.5*f;
    return f;
}

struct PixelOut
{
    float4 color : SV_Target;
};

PixelOut PS(float4 posH : SV_POSITION,
            float2 iUV  : UVOUT,
            float3 iPos : POSOUT) {

    PixelOut pixel;

    float2 uv = iUV;
    float2 q = float2(uv.x, uv.y);

    q.x *= 5.;
    q.y *= 2.;

    float strength = floor(q.x + 1.);
    float T3 = max(3., 1.25*strength)*time;
    q.x = (q.x % 1.0) - 0.5;
    q.y -= 0.25;
    float n = fbm(strength*q - float2(0, T3));
    float c = 1. - 16. * pow(max(0., length(q*float2(1.8 + q.y*1.5, .75)) - n * max(0., q.y + .25)), 1.2);
    float c1 = n * c * (1.5 - pow(2.50*uv.y, 4.));
    c1 = clamp(c1, 0., 1.);

    float3 col = float3(1.5*c1, 1.5*c1*c1*c1, c1*c1*c1*c1*c1*c1);

    float a = c * (1. - pow((uv.y), 3.));
    pixel.color = float4(lerp(float3(0., 0., 0.), col.xxx*fireColor, a), 1.0);

    //pixel.color = float4(1.0, 0.0, 0.0, 1.0);
    if (!(pixel.color.r > 0.25 || pixel.color.g > 0.25 || pixel.color.b > 0.25)) {
        discard;
    }
    return pixel;
}
