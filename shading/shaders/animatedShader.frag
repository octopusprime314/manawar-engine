#version 430

layout(location = 0) out vec4 out_1;
layout(location = 1) out vec4 out_2;
layout(location = 2) out vec4 out_3;

in VsData
{
	vec3 normalOut;			   // Transformed normal based on the normal matrix transform
	vec2 textureCoordinateOut; // Passthrough
	vec3 positionOut;          // Passthrough for deferred shadow rendering
}  vsData;


uniform sampler2D textureMap;   //Texture data array

void main(){

	out_1 = vec4(texture(textureMap, vsData.textureCoordinateOut).rgb, 1.0);
	out_2 = vec4(normalize(vsData.normalOut), 1.0);
	out_3 = vec4(vsData.positionOut.xyz, 1.0);
}