#version 330

in vec3 normalOut;			 // Transformed normal based on the vertex shader
in vec3 directionalLight;
void main(){

	//Arbitrary color
	vec4 color = vec4(1.0, 0.0, 0.0, 1.0);

	vec3 normalizedNormal = normalize(normalOut.xyz);
	vec3 lightNormal = normalize(directionalLight.xyz);

	//If the dot product between the two vectors is greater than 0 then that directional light will illuminate the fragment 
	float illumination = (lightNormal.x * normalizedNormal.x) + (lightNormal.y * normalizedNormal.y) + (lightNormal.z * normalizedNormal.z);
	if(illumination > 0.0){
		gl_FragColor = color * illumination;
	}
	else{
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}