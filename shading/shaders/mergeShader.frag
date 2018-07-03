#version 430

out vec4 fragColor;
in VsData
{
	vec2 texCoordOut;
} vsData;

uniform sampler2D deferredTexture;  
uniform sampler2D velocityTexture;   

void main(){
    
	
	//const float gamma = 2.2;
    //vec3 hdrColor = texture(deferredTexture, vsData.texCoordOut).rgb;      
    //vec3 bloomColor = texture(bloomTexture, vsData.texCoordOut).rgb;
    //hdrColor += bloomColor; // additive blending
    
	
	// tone mapping
    //vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it       
    //result = pow(result, vec3(1.0 / gamma));
	
	
    //fragColor = vec4(hdrColor, 1.0);
	
	//Divide by 2 to prevent overblurring
	vec2 velocityVector = texture(velocityTexture, vsData.texCoordOut).xy / 2.0;

    vec4 result = vec4(0.0);
	vec2 texCoords = vsData.texCoordOut;
	
    result += texture(deferredTexture, texCoords) * 0.4;
    texCoords -= velocityVector;
    result += texture(deferredTexture, texCoords) * 0.3;
    texCoords -= velocityVector;
    result += texture(deferredTexture, texCoords) * 0.2;
    texCoords -= velocityVector;
    result += texture(deferredTexture, texCoords) * 0.1;

    fragColor = vec4(vec3(result.rgb), 1.0);
}