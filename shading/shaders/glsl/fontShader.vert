#version 430

layout(location = 0) in vec3 vp;
layout(location = 1) in vec2 tc;

out VsData { vec2 fragTexCoord; }
vsData;

void main() {
    gl_Position         = vec4(vp, 1.0);
    vsData.fragTexCoord = tc;
}