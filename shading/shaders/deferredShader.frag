#version 330

uniform sampler2D diffuseTexture;   //Diffuse texture data array
uniform sampler2D normalTexture;    //Normal texture data array
uniform sampler2D positionTexture;  //Position texture data array
uniform sampler2D cameraDepthTexture;   //depth texture data array with values 1.0 to 0.0, with 0.0 being closer
uniform sampler2D mapDepthTexture;      //depth texture data array with values 1.0 to 0.0, with 0.0 being closer
uniform samplerCube depthMap;		//cube depth map for point light shadows

uniform mat4 lightViewMatrix;     	//Light perspective's view matrix
uniform mat4 lightMapViewMatrix;    //Light perspective's view matrix
uniform mat4 viewToModelMatrix;		//Inverse camera view space matrix

uniform vec3 pointLightPositions[20];//max lights is 20 for now
uniform vec3 pointLightColors[20]; //max lights is 20 for now
uniform float pointLightRanges[20];//max lights is 20 for now
uniform int  numPointLights;

uniform int views;   //views set to 0 is diffuse mapping, set to 1 is shadow mapping and set to 2 is normal mapping

uniform vec3 light;  
in vec2 textureCoordinateOut; // Passthrough

vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

float ambient = 0.4;

void main(){

	//extract position from position texture
	vec4 position = texture(positionTexture, textureCoordinateOut.xy);
	
	if(position.x == 0.0 && position.y == 0.0 && position.z == 0.0){
		discard;
	}
	
	//extract normal from normal texture
	vec3 normalizedNormal = normalize(texture(normalTexture, textureCoordinateOut.xy).xyz); 
	//extract color from diffuse texture
	vec4 diffuse = texture(diffuseTexture, textureCoordinateOut.xy);
	
	//Directional light calculation
	//NEED to invert light vector other a normal surface pointing up with a light pointing
	//down would result in a negative dot product of the two vecs, inverting gives us positive numbers!
	vec3 normalizedLight = normalize(-light);
	float illumination = dot(normalizedLight, normalizedNormal);
	
	//Convert from camera space vertex to light clip space vertex
	vec4 shadowMapping = lightViewMatrix * vec4(position.xyz, 1.0);
    shadowMapping = shadowMapping/shadowMapping.w; 
    vec2 shadowTextureCoordinates = shadowMapping.xy * vec2(0.5,0.5) + vec2(0.5,0.5);

	if(views == 0){
		gl_FragColor = vec4(diffuse.rgb, 1.0);
	}
	else if(views == 1){
		float depth = texture(mapDepthTexture, textureCoordinateOut).x;
		gl_FragColor = vec4(depth, depth, depth, 1.0);
	}
	else if(views == 2){		
		//float depth = texture(cameraDepthTexture, textureCoordinateOut).x;
		//gl_FragColor = vec4(depth, depth, depth, 1.0);
		
		vec3 cubeMapTexCoords = (viewToModelMatrix * vec4(position.xyz,1.0)).xyz - pointLightPositions[0].xyz;
		float cubeDepth = texture(depthMap, normalize(cubeMapTexCoords.xyz)).x;
		gl_FragColor = vec4(vec3(cubeDepth), 1.0);
		//Records cube map texture indexing to look at in Renderdoc
		//gl_FragColor = vec4(cubeMapTexCoords, 1.0);
	}
	else if(views == 3){
		gl_FragColor = vec4(diffuse.rgb * min(illumination + ambient, 1.0), 1.0);
	}
	else if(views == 4){
		gl_FragColor = vec4(normalizedNormal.xyz, 1.0);
	}
	else if(views == 5){
		gl_FragColor = vec4(normalize(position.xyz), 1.0);
	}
	else if(views == 6){
		const float bias = 0.005; //removes shadow acne by adding a small bias
		
		//Only shadow in textures space
		if(shadowTextureCoordinates.x <= 1.0 && shadowTextureCoordinates.x >= 0.0 && shadowTextureCoordinates.y <= 1.0 && shadowTextureCoordinates.y >= 0.0){
			
			float shadow = 1.0;
			if ( texture( cameraDepthTexture, shadowTextureCoordinates).x < (shadowMapping.z * 0.5 + 0.5) - bias){
					shadow = ambient;
			}	
			gl_FragColor = vec4(diffuse.rgb * shadow * min(illumination + ambient, 1.0), 1.0);
		}
		else{
			vec4 shadowMappingMap = lightMapViewMatrix * vec4(position.xyz, 1.0);
			shadowMappingMap = shadowMappingMap/shadowMappingMap.w; 
			vec2 shadowTextureCoordinatesMap = shadowMappingMap.xy * vec2(0.5,0.5) + vec2(0.5,0.5);
			
			float shadow = 1.0;
			if ( texture( mapDepthTexture, shadowTextureCoordinatesMap).x < (shadowMappingMap.z * 0.5 + 0.5) - bias){
					shadow = ambient;
			}	
			
			gl_FragColor = vec4(diffuse.rgb * shadow * min(illumination + ambient, 1.0), 1.0);
		}
	}
	//Show light positions
	else if(views == 7){
	
		
		
		float lightAccumulation = 0.0;
		vec3 pointLighting = vec3(0.0, 0.0, 0.0);
		//for(int i = 0; i < numPointLights; i++){
			vec3 pointLightDir = (viewToModelMatrix * vec4(position.xyz,1.0)).xyz - pointLightPositions[0].xyz;
			float distanceFromLight = length(pointLightDir);
			if(distanceFromLight <= pointLightRanges[0]){
				vec3 pointLightDirNorm = normalize(-pointLightDir);
				pointLighting += (dot(pointLightDirNorm, normalizedNormal)) * (1.0 - (4.0*distanceFromLight/pointLightRanges[0])) * pointLightColors[0];
			}
		//}
		
		vec3 cubeMapTexCoords = (viewToModelMatrix * vec4(position.xyz,1.0)).xyz - pointLightPositions[0].xyz;
		float distance = length(cubeMapTexCoords);
		float cubeDepth = texture(depthMap, normalize(cubeMapTexCoords.xyz)).x*pointLightRanges[0];
		float bias = 0.05; 
		float shadow = distance - bias > cubeDepth ? ambient : 1.0;
		//gl_FragColor = vec4(diffuse.rgb * shadow * min(pointLighting + ambient, 1.0), 1.0);
		gl_FragColor = vec4(diffuse.rgb * shadow * min(ambient, 1.0), 1.0);
	}
	gl_FragDepth = 0.0;
}