#version 330

in vec3 normalOut;			 // Transformed normal based on the vertex shader
in vec2 textureCoordinateOut;// Texture coordinate
in vec3 positionOut; 

layout(location = 0) out vec4 out_1;
layout(location = 1) out vec4 out_2;
layout(location = 2) out vec4 out_3;

uniform sampler2D textureMap;   //Texture data array

void main(){

	out_1 = vec4(texture(textureMap, textureCoordinateOut).rgb, 1.0);
	out_2 = vec4(normalize(normalOut), 1.0);
	out_3 = vec4(positionOut.xyz, 1.0);
}