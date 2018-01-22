#version 330

in vec3 normalOut;			 // Transformed normal based on the vertex shader
in vec2 textureCoordinateOut;// Texture coordinate
in vec3 positionOut; 

uniform sampler2D textureMap;   //Texture data array

void main(){

	gl_FragData[0] = vec4(texture(textureMap, textureCoordinateOut).rgb, 1.0);
	gl_FragData[1] = vec4(normalize(normalOut), 1.0);
	gl_FragData[2] = vec4(positionOut.xyz, 1.0);
}