#version 400

layout (location = 0) in vec3 vp;
layout (location = 1) in vec2 tc;

out vec2 fragTexCoord;

void main() 
{
  gl_Position = vec4(vp, 1.0);
  fragTexCoord = tc;
}