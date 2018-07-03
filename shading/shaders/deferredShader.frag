#version 430

uniform sampler2D diffuseTexture;   //Diffuse texture data array
uniform sampler2D normalTexture;    //Normal texture data array
uniform sampler2D positionTexture;  //Position texture data array
uniform sampler2D velocityTexture;      //velocity texture data array
uniform sampler2D cameraDepthTexture;   //depth texture data array with values 1.0 to 0.0, with 0.0 being closer
uniform sampler2D mapDepthTexture;      //depth texture data array with values 1.0 to 0.0, with 0.0 being closer
uniform samplerCube depthMap;		//cube depth map for point light shadows
uniform samplerCube skyboxDayTexture;   //skybox day
uniform samplerCube skyboxNightTexture;	//skybox night
uniform sampler2D   ssaoTexture;      //depth texture data array with values 1.0 to 0.0, with 0.0 being closer
//uniform samplerCube environmentMapTexture;      //depth texture data array with values 1.0 to 0.0, with 0.0 being closer

uniform mat4 lightViewMatrix;     	//Light perspective's view matrix
uniform mat4 lightMapViewMatrix;    //Light perspective's view matrix
uniform mat4 viewToModelMatrix;		//Inverse camera view space matrix

uniform vec3 pointLightPositions[20];//max lights is 20 for now
uniform vec3 pointLightColors[20]; //max lights is 20 for now
uniform float pointLightRanges[20];//max lights is 20 for now
uniform int  numPointLights;

uniform int views;   //views set to 0 is diffuse mapping, set to 1 is shadow mapping and set to 2 is normal mapping
uniform vec3 light;  
uniform float farPlane;

in VsData
{
	vec2 texCoordOut; 
	vec3 vsViewDirection;
} vsData;

vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

float ambient = 0.3;
float shadowEffect = 0.6;
out vec4 fragColor;

void main(){

	//extract position from position texture
	vec4 position = texture(positionTexture, vsData.texCoordOut.xy);
	//extract normal from normal texture
	vec3 normalizedNormal = normalize(texture(normalTexture, vsData.texCoordOut.xy).xyz); 
	//extract color from diffuse texture
	vec4 diffuse = texture(diffuseTexture, vsData.texCoordOut.xy);
	//extract 2d velocity buffer
	vec2 velocity = texture(velocityTexture, vsData.texCoordOut.xy).rg;
	
	float occlusion = texture(ssaoTexture, vsData.texCoordOut.xy).r;
	
	if(position.x != 0.0 && position.y != 0.0 && position.z != 0.0){
		gl_FragDepth = (length(position.xyz)/farPlane) / 2.0f;
	}
	else{
		gl_FragDepth = 0.5f;
	}
	
	//Directional light calculation
	//NEED to invert light vector other a normal surface pointing up with a light pointing
	//down would result in a negative dot product of the two vecs, inverting gives us positive numbers!
	vec3 normalizedLight = normalize(-light);
	float illumination = dot(normalizedLight, normalizedNormal);
	
	//Convert from camera space vertex to light clip space vertex
	vec4 shadowMapping = lightViewMatrix * vec4(position.xyz, 1.0);
    shadowMapping = shadowMapping/shadowMapping.w; 
    vec2 shadowTextureCoordinates = shadowMapping.xy * vec2(0.5,0.5) + vec2(0.5,0.5);
	
	vec4 shadowMappingMap = lightMapViewMatrix * vec4(position.xyz, 1.0);
	shadowMappingMap = shadowMappingMap/shadowMappingMap.w; 
	vec2 shadowTextureCoordinatesMap = shadowMappingMap.xy * vec2(0.5,0.5) + vec2(0.5,0.5);

	if(views == 0 || views == 8){
		if(position.x == 0.0 && position.y == 0.0 && position.z == 0.0){
			vec4 dayColor = texture(skyboxDayTexture, vec3(vsData.vsViewDirection.x, -vsData.vsViewDirection.y, vsData.vsViewDirection.z));
			vec4 nightColor = texture(skyboxNightTexture, vec3(vsData.vsViewDirection.x, -vsData.vsViewDirection.y, vsData.vsViewDirection.z));
			fragColor = (((1.0 - light.y)/2.0) * dayColor) + (((1.0 + light.y)/2.0) * nightColor);
		}
		else {
			vec3 pointLighting = vec3(0.0, 0.0, 0.0);
			float totalShadow = 1.0;
			float directionalShadow = 1.0;
			float pointShadow = 1.0;
			//illumination is from directional light but we don't want to illuminate when the sun is past the horizon
			//aka night time
			if(light.y <= 0.0) {
				const float bias = 0.005; //removes shadow acne by adding a small bias
				//Only shadow in textures space
				if(shadowTextureCoordinates.x <= 1.0 && shadowTextureCoordinates.x >= 0.0 && shadowTextureCoordinates.y <= 1.0 && shadowTextureCoordinates.y >= 0.0){
					
					if ( texture( cameraDepthTexture, shadowTextureCoordinates).x < (shadowMapping.z * 0.5 + 0.5) - bias){
						directionalShadow = shadowEffect;
					}	
				}
				else if(shadowTextureCoordinatesMap.x <= 1.0 && shadowTextureCoordinatesMap.x >= 0.0 && shadowTextureCoordinatesMap.y <= 1.0 && shadowTextureCoordinatesMap.y >= 0.0){
					vec4 shadowMappingMap = lightMapViewMatrix * vec4(position.xyz, 1.0);
					shadowMappingMap = shadowMappingMap/shadowMappingMap.w; 
					vec2 shadowTextureCoordinatesMap = shadowMappingMap.xy * vec2(0.5,0.5) + vec2(0.5,0.5);
					
					if ( texture( mapDepthTexture, shadowTextureCoordinatesMap).x < (shadowMappingMap.z * 0.5 + 0.5) - bias){
						directionalShadow = shadowEffect;
					}	
				}
				
			}
			else {
				illumination = 0.0;
			}
			
			//Point lights always emit light versus directional sun shadows
			float numLights = numPointLights;
			float totalPointLightEffect = 0.0;
			for(int i = 0; i < numPointLights; i++){
				vec3 pointLightDir = position.xyz - pointLightPositions[i].xyz;
				float distanceFromLight = length(pointLightDir);
				float bias = 0.1; 
				if(distanceFromLight < pointLightRanges[i]){
					vec3 pointLightDirNorm = normalize(-pointLightDir);
					pointLighting += (dot(pointLightDirNorm, normalizedNormal)) * (1.0 - (distanceFromLight/(pointLightRanges[i]))) * pointLightColors[i];
					totalPointLightEffect += dot(pointLightDirNorm, normalizedNormal) * (1.0 - (distanceFromLight/(pointLightRanges[i])));
					
					vec3 cubeMapTexCoords = (viewToModelMatrix * vec4(position.xyz,1.0)).xyz - (viewToModelMatrix * vec4(pointLightPositions[i].xyz, 1.0)).xyz;
					float distance = length(cubeMapTexCoords);
					float cubeDepth = texture(depthMap, normalize(cubeMapTexCoords.xyz)).x*pointLightRanges[i];
					
					if(cubeDepth + bias < distance){
						//pointShadow -= ((1.0 - pointLightShadowEffect)/numLights)*(1.0 - (distance/cubeDepth));
						//pointShadow -= ((1.0 - shadowEffect)/numLights);
						//USE ONLY ONE POINT SHADOW FOR NOW!!!!
						pointShadow -= (1.0 - shadowEffect);						
					}					
				}
			}
		
			totalShadow = min(directionalShadow, pointShadow);
		
			//If all light components add up to more than one, then normalize
			//light components other than ambient
			//if(illumination + totalPointLightEffect + ambient > 1.0){
			//	vec2 lightNormalized = normalize(vec2(illumination, totalPointLightEffect));
			//	illumination = lightNormalized.x - (ambient / 2);
			//	pointLighting = (pointLighting * lightNormalized.y) - (ambient / 2);
			//}
			
			vec3 lightComponentIllumination = (illumination  * diffuse.rgb * directionalShadow) + 
											  (pointLighting * diffuse.rgb * pointShadow);
			
			fragColor = vec4((lightComponentIllumination) + (ambient * diffuse.rgb * occlusion), 1.0);
		}
	}
	else if(views == 1){		
		
		fragColor = vec4(diffuse.rgb, 1.0);
	}
	else if(views == 2){
		
		fragColor = vec4(normalizedNormal.xyz, 1.0);
	}
	else if(views == 3){
	
		fragColor = vec4(normalize(position.xyz), 1.0);
	}
	else if(views == 4){
	
		fragColor = vec4(occlusion, occlusion, occlusion, 1.0); 
	}
	else if(views == 5){
		float depth = texture(mapDepthTexture, vsData.texCoordOut).x;
		fragColor = vec4(depth, depth, depth, 1.0);
	}
	else if(views == 6){
		vec3 cubeMapTexCoords = (viewToModelMatrix * vec4(position.xyz,1.0)).xyz - (viewToModelMatrix * vec4(pointLightPositions[0].xyz, 1.0)).xyz;
		float cubeDepth = texture(depthMap, normalize(cubeMapTexCoords.xyz)).x;
		fragColor = vec4(vec3(cubeDepth), 1.0);
	}
	else if(views == 7){
		fragColor = vec4(velocity, 0.0, 1.0);	
		//fragColor = vec4(texture(environmentMapTexture, vec3(vsData.vsViewDirection.x, vsData.vsViewDirection.y, vsData.vsViewDirection.z)).rgb, 1.0);
	}
}