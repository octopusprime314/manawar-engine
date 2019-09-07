#version 430

in VsData {
    vec3 normalOut;            // Transformed normal based on the normal matrix transform
    vec2 textureCoordinateOut; // Passthrough
    vec3 positionOut;          // Passthrough for deferred shadow rendering
    vec4 projPositionOut;      // For computing velocity buffer
    vec4 prevProjPositionOut;  // For computing velocity buffer
}
vsData;

layout(location = 0) out vec4 out_1;
layout(location = 1) out vec4 out_2;
layout(location = 2) out vec4 out_3;
layout(location = 3) out vec4 out_4;

uniform sampler2D textureMap; // Texture data array
uniform sampler2D tex0;       // red designated texture from alphatex0
uniform sampler2D tex1;       // green designated texture from alphatex0
uniform sampler2D tex2;       // blue designated texture from alphatex0
uniform sampler2D tex3;       // alpha designeated texture from alphatex0
uniform sampler2D alphatex0;  // texture indicating which red, green, blue, alpha or background texture to choose

uniform int isLayeredTexture;
uniform int primitiveOffset;
uniform int id;

void main() {

    vec4 vTexColor = vec4(0.0, 0.0, 0.0, 1.0);

    if (isLayeredTexture == 1) {

        vec4 red   = texture2D(tex0, vsData.textureCoordinateOut.st * 150);
        vec4 green = texture2D(tex1, vsData.textureCoordinateOut.st * 150);
        vec4 blue  = texture2D(tex2, vsData.textureCoordinateOut.st * 150);
        vec4 alpha = texture2D(tex3, vsData.textureCoordinateOut.st * 150);
        // alpha map for textures
        vec4 idTexture = texture2D(alphatex0, vsData.textureCoordinateOut.st);

        // Used to prevent tiling
        vec4  tRandoR    = texture2D(tex0, vsData.textureCoordinateOut.st * 5.0); // red
        vec4  tRandoG    = texture2D(tex1, vsData.textureCoordinateOut.st * 5.0); // green
        vec4  tRandoB    = texture2D(tex2, vsData.textureCoordinateOut.st * 5.0); // blue
        vec4  tRandoA    = texture2D(tex3, vsData.textureCoordinateOut.st * 5.0); // alpha
        vec4  tR         = red * tRandoR;
        vec4  tG         = green * tRandoG;
        vec4  tB         = blue * tRandoB;
        vec4  tA         = alpha * tRandoA;
        float redPixel   = idTexture.r;
        float greenPixel = idTexture.g;
        float bluePixel  = idTexture.b;
        float alphaPixel = 1.0f - idTexture.a; // alpha value is inverse

        vec4 lerpComponent = vec4(0.0, 0.0, 0.0, 1.0);
        // Path next
        lerpComponent = redPixel * tR + (1.0f - redPixel) * lerpComponent;
        // Mountain next
        lerpComponent = greenPixel * tG + (1.0f - greenPixel) * lerpComponent;
        // Dirt next
        lerpComponent = bluePixel * tB + (1.0f - bluePixel) * lerpComponent;
        // snow next
        lerpComponent = alphaPixel * tA + (1.0f - alphaPixel) * lerpComponent;

        vTexColor = vec4(lerpComponent.rgb, 1.0);
    } else {
        vTexColor = vec4(texture(textureMap, vsData.textureCoordinateOut).rgb, 1.0);
    }

    // Velocity buffer
    vec2 currProjPos = (vsData.projPositionOut.xy / vsData.projPositionOut.w);
    vec2 prevProjPos = (vsData.prevProjPositionOut.xy / vsData.prevProjPositionOut.w);

    out_1 = vec4(vTexColor.rgb, 1.0);
    // write the primitive/triangle id to the alpha value and scale by 2^24 allowing 16 million unique triangles to id
    out_2 = vec4(normalize(vsData.normalOut), 1.0);
    out_3 = vec4(vec2(currProjPos - prevProjPos),
                 float(id) / 16777216.0f,
                 float(gl_PrimitiveID + primitiveOffset) / 16777216.0f);
}