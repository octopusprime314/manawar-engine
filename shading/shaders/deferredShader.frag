#version 330

uniform sampler2D diffuseTexture;   //Diffuse texture data array
uniform sampler2D normalTexture;    //Normal texture data array
uniform sampler2D positionTexture;  //Position texture data array
uniform sampler2D staticDepthTexture;     //depth texture data array with values 1.0 to 0.0, with 1.0 being closer
uniform sampler2D animatedDepthTexture;     //depth texture data array with values 1.0 to 0.0, with 1.0 being closer
uniform sampler2D mapDepthTexture;     //depth texture data array with values 1.0 to 0.0, with 1.0 being closer

uniform mat4 lightViewMatrix;     	//Light perspective's view matrix
uniform mat4 lightMapViewMatrix;     	//Light perspective's view matrix
uniform mat4 projection;     		//Light perspective's projection matrix
uniform mat4 view;     				//Camera perspective's view matrix
uniform vec3 cameraPosition;

uniform int views;   //views set to 0 is diffuse mapping, set to 1 is shadow mapping and set to 2 is normal mapping

uniform vec3 light;  
in vec2 textureCoordinateOut; // Passthrough

void main(){

	//extract normal from normal texture
	vec3 normalizedNormal = normalize(texture(normalTexture, textureCoordinateOut.xy).xyz); 
	//extract color from diffuse texture
	vec4 diffuse = texture(diffuseTexture, textureCoordinateOut.xy);
	//extract position from position texture
	vec4 position = texture(positionTexture, textureCoordinateOut.xy);
	
	//Directional light calculation
	vec3 normalizedLight = normalize(light);
	float illumination = dot(normalizedLight, normalizedNormal);
	
	//Convert from camera space vertex to light clip space vertex
	vec4 shadowMapping = lightViewMatrix * vec4(position.xyz, 1.0);
    shadowMapping = shadowMapping/shadowMapping.w; 
    vec2 shadowTextureCoordinates = shadowMapping.xy * vec2(0.5,0.5) + vec2(0.5,0.5);

	if(views == 0){
		gl_FragColor = vec4(diffuse.rgb, 1.0);
	}
	else if(views == 1){
	
		vec4 shadowMappingMap = lightMapViewMatrix * vec4(position.xyz, 1.0);
		shadowMappingMap = shadowMappingMap/shadowMappingMap.w; 
		vec2 shadowTextureCoordinatesMap = shadowMappingMap.xy * vec2(0.5,0.5) + vec2(0.5,0.5);
		vec4 depth = texture(mapDepthTexture, textureCoordinateOut);
		gl_FragColor = vec4(depth.xyz, 1.0);
	}
	else if(views == 2){
	
		if(shadowTextureCoordinates.x <= 1.0 && shadowTextureCoordinates.x >= 0.0 && shadowTextureCoordinates.y <= 1.0 && shadowTextureCoordinates.y >= 0.0){
			vec4 depthMap = texture(staticDepthTexture, shadowTextureCoordinates);
			vec4 depth = depthMap;
			gl_FragColor = vec4(depth.xyz, 1.0);
		}
		else{
			gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
		}
	}
	else if(views == 3){
		gl_FragColor = vec4(diffuse.rgb * illumination, 1.0);
		//gl_FragColor = vec4(normalizedNormal.xyz, 1.0);
	}
	else if(views == 4){
		gl_FragColor = vec4(normalize(position.xyz), 1.0);
	}
	else if(views == 5){
	
		const float bias = 0.0001; //removes shadow acne by adding a small bias
		
		
		
		//Only shadow in textures space
		if(shadowTextureCoordinates.x <= 1.0 && shadowTextureCoordinates.x >= 0.0 && shadowTextureCoordinates.y <= 1.0 && shadowTextureCoordinates.y >= 0.0){
			vec4 depthStatic = texture(staticDepthTexture, shadowTextureCoordinates);
			vec4 depthAnimated = texture(animatedDepthTexture, shadowTextureCoordinates);
			float depthValue = 0.0;
			if(depthStatic.z <= depthAnimated.z || depthAnimated.z == 0.0){
				depthValue = depthStatic.z;
			}
			else if(depthStatic.z > depthAnimated.z || depthStatic.z == 0.0){
				depthValue = depthAnimated.z;
			}
			depthValue = depthValue + bias;
			float shadow = 0.2;
			if((shadowMapping.z * 0.5 + 0.5) < depthValue){
				shadow = 1.0;
			}
			gl_FragColor = vec4(diffuse.rgb * shadow * illumination, 1.0);
		}
		else{
			vec4 shadowMappingMap = lightMapViewMatrix * vec4(position.xyz, 1.0);
			shadowMappingMap = shadowMappingMap/shadowMappingMap.w; 
			vec2 shadowTextureCoordinatesMap = shadowMappingMap.xy * vec2(0.5,0.5) + vec2(0.5,0.5);
			
			vec4 depth = texture(mapDepthTexture, shadowTextureCoordinatesMap);
			
			float depthValue = depth.z + bias;
			float shadow = 0.2;
			if((shadowMappingMap.z * 0.5 + 0.5) < depthValue){
				shadow = 1.0;
			}
			gl_FragColor = vec4(diffuse.rgb * shadow * illumination, 1.0);
			
		}
	}
}