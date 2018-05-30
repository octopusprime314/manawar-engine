#version 330

uniform sampler2D   textureMap;   //Texture data array
uniform sampler2D   cameraDepthTexture;   //depth texture data array with values 1.0 to 0.0, with 0.0 being closer
uniform sampler2D   mapDepthTexture;      //depth texture data array with values 1.0 to 0.0, with 0.0 being closer
uniform samplerCube depthMap;		//cube depth map for point light shadows

uniform mat4 lightViewMatrix;     	//Light perspective's view matrix
uniform mat4 lightMapViewMatrix;    //Light perspective's view matrix
uniform mat4 viewToModelMatrix;		//Inverse camera view space matrix

uniform vec3  pointLightPositions[20];//max lights is 20 for now
uniform vec3  pointLightColors[20]; //max lights is 20 for now
uniform float pointLightRanges[20];//max lights is 20 for now
uniform int   numPointLights;

float ambient = 0.1;
float shadowEffect = 0.6;
float pointLightShadowEffect = 0.2;


in vec3 normalOut;			 // Transformed normal based on the vertex shader
in vec2 textureCoordinateOut;// Texture coordinate
in vec4 positionOut; 
out vec4 fragColor;
uniform float farPlane;
uniform vec3 light;  

void main(){

	//vec4 diffuse = texture(textureMap, textureCoordinateOut);
	////If any transparency just discard this fragment
	//if(diffuse.a < 0.1) {
	//	discard;
	//}
	//else {
	//	gl_FragDepth = (length(positionOut.xyz)/farPlane) / 2.0f;
	//}
	//fragColor = diffuse;
	
	
	vec4 diffuse = texture(textureMap, textureCoordinateOut);
	//If any transparency just discard this fragment
	if(diffuse.a < 0.1) {
		discard;
	}
	else {
		
		//Directional light calculation
		//NEED to invert light vector other a normal surface pointing up with a light pointing
		//down would result in a negative dot product of the two vecs, inverting gives us positive numbers!
		vec3 normalizedLight = normalize(-light);
		float illumination = dot(normalizedLight, normalOut);
		
		//Convert from camera space vertex to light clip space vertex
		vec4 shadowMapping = lightViewMatrix * vec4(positionOut.xyz, 1.0);
		shadowMapping = shadowMapping/shadowMapping.w; 
		vec2 shadowTextureCoordinates = shadowMapping.xy * vec2(0.5,0.5) + vec2(0.5,0.5);
		
		vec4 shadowMappingMap = lightMapViewMatrix * vec4(positionOut.xyz, 1.0);
		shadowMappingMap = shadowMappingMap/shadowMappingMap.w; 
		vec2 shadowTextureCoordinatesMap = shadowMappingMap.xy * vec2(0.5,0.5) + vec2(0.5,0.5);
		
		//Add normal shading stuff here
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
				vec4 shadowMappingMap = lightMapViewMatrix * vec4(positionOut.xyz, 1.0);
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
			vec3 pointLightDir = positionOut.xyz - pointLightPositions[i].xyz;
			float distanceFromLight = length(pointLightDir);
			if(distanceFromLight < pointLightRanges[i]){
				vec3 pointLightDirNorm = normalize(-pointLightDir);
				pointLighting += (dot(pointLightDirNorm, normalOut)) * (1.0 - (distanceFromLight/(pointLightRanges[i]))) * pointLightColors[i];
				totalPointLightEffect += dot(pointLightDirNorm, normalOut) * (1.0 - (distanceFromLight/(pointLightRanges[i])));
				
				vec3 cubeMapTexCoords = (viewToModelMatrix * vec4(positionOut.xyz,1.0)).xyz - (viewToModelMatrix * vec4(pointLightPositions[i].xyz, 1.0)).xyz;
				float distance = length(cubeMapTexCoords);
				float cubeDepth = texture(depthMap, normalize(cubeMapTexCoords.xyz)).x*pointLightRanges[i];
				float bias = 0.05; 
				if(cubeDepth + bias < distance){
					//pointShadow -= ((1.0 - pointLightShadowEffect)/numLights)*(1.0 - (distance/cubeDepth));
					pointShadow -= ((1.0 - shadowEffect)/numLights);			
				}	
				
			}
		}
	
		totalShadow = min(directionalShadow, pointShadow);
	
		//If all light components add up to more than one, then normalize
		//light components other than ambient
		if(illumination + totalPointLightEffect + ambient > 1.0){
			vec2 lightNormalized = normalize(vec2(illumination, totalPointLightEffect));
			illumination = lightNormalized.x - (ambient / 2);
			pointLighting = (pointLighting * lightNormalized.y) - (ambient / 2);
		}

		vec3 lightComponentIllumination = (illumination  * diffuse.rgb) + 
										  (pointLighting * diffuse.rgb);
		
		fragColor = vec4((lightComponentIllumination * totalShadow) + (ambient * diffuse.rgb), 1.0);
	
		gl_FragDepth = (length(positionOut.xyz)/farPlane) / 2.0f;
	}
}