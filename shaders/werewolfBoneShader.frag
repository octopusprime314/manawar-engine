#version 330

in vec2 textureCoordinateOut;// Texture coordinate
in vec3 normalOut;			 // Transformed normal based on the vertex shader
in vec3 directionalLight;

uniform sampler2D textureMap;   //Texture data array

void main(){

	vec4 fragmentColor = texture(textureMap, textureCoordinateOut);

	vec3 normalizedNormal = normalize(normalOut.xyz);
	vec3 lightNormal = normalize(directionalLight.xyz);
	
	gl_FragColor = fragmentColor;

	//If the dot product between the two vectors is greater than 0 then that directional light will illuminate the fragment 
	//float illumination = (lightNormal.x * normalizedNormal.x) + (lightNormal.y * normalizedNormal.y) + (lightNormal.z * normalizedNormal.z);
	//if(illumination > 0.0){
	//	gl_FragColor = fragmentColor * illumination;
	//}
	//else{
	//	gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	//}
}