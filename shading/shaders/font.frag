#version 430

uniform sampler2D tex;
in VsData
{
	vec2 fragTexCoord;
}  vsData;
out vec4 fragColor;

void main() 
{
	fragColor = texture (tex, vec2(vsData.fragTexCoord.x, vsData.fragTexCoord.y));
}