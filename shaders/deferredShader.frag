#version 330

uniform sampler2D diffuseTexture;   //Diffuse texture data array
uniform sampler2D normalTexture;    //Normal texture data array
uniform vec3 light;     			//Light vector

in vec2 textureCoordinateOut; // Passthrough

void main(){

	//extract normal from normal texture
	vec3 normalizedNormal = normalize(texture(normalTexture, textureCoordinateOut.xy).xyz); 
	//extract color from diffuse texture
	vec4 diffuse = texture(diffuseTexture, textureCoordinateOut.xy);
	
	float illumination = (light.x * normalizedNormal.x) + (light.y * normalizedNormal.y) + (light.z * normalizedNormal.z);
	
	if(illumination > 0.0){
		gl_FragColor = vec4(vec3(diffuse.rgb * illumination), 1.0);
	}
	else{
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}