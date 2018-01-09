#version 330

in vec2 textureCoordinateOut;// Texture coordinate
uniform sampler2D textureMap;   //Texture data array

void main(){
	
	gl_FragColor = texture(textureMap, textureCoordinateOut);
}