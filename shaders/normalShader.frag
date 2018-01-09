#version 330

in vec3 normalOut;			 // Passthrough

void main(){

	vec3 normalizedNormal = normalize(normalOut.xyz);
	gl_FragColor = vec4(normalizedNormal, 1.0);
}