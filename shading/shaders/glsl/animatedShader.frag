#version 430

layout(location = 0) out vec4 out_1;
layout(location = 1) out vec4 out_2;
layout(location = 2) out vec4 out_3;
layout(location = 3) out vec4 out_4;

in VsData {
    vec3 normalOut;            // Transformed normal based on the normal matrix transform
    vec2 textureCoordinateOut; // Passthrough
    vec3 positionOut;          // Passthrough for deferred shadow rendering
    vec4 projPositionOut;      // For computing velocity buffer
    vec4 prevProjPositionOut;  // For computing velocity buffer
}
vsData;

uniform int id;
uniform sampler2D textureMap; // Texture data array

void main() {

    vec2 currProjPos = (vsData.projPositionOut.xy / vsData.projPositionOut.w);
    vec2 prevProjPos = (vsData.prevProjPositionOut.xy / vsData.prevProjPositionOut.w);

    out_1 = vec4(texture(textureMap, vsData.textureCoordinateOut).rgb, 1.0);
    // write the primitive/triangle id to the alpha value and scale by 2^24 allowing 16 million unique triangles to id
    out_2 = vec4(normalize(vsData.normalOut), 1.0);
    out_3 = vec4(vec2(currProjPos - prevProjPos), float(id) / 16777216.0f, float(gl_PrimitiveID) / 16777216.0f);
}