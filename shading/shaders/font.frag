#version 400

uniform sampler2D tex;
in vec2 fragTexCoord;
out vec4 fragColor;

void main() 
{
	fragColor = texture (tex, vec2(fragTexCoord.x, fragTexCoord.y));
}