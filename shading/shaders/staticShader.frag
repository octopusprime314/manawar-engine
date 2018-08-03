#version 430

in VsData
{
	vec3 normalOut;			   // Transformed normal based on the normal matrix transform
	vec2 textureCoordinateOut; // Passthrough
	vec3 positionOut;          // Passthrough for deferred shadow rendering
	vec4 projPositionOut;      // For computing velocity buffer
	vec4 prevProjPositionOut;  // For computing velocity buffer
} vsData;

layout(location = 0) out vec4 out_1;
layout(location = 1) out vec4 out_2;
layout(location = 2) out vec4 out_3;
layout(location = 3) out vec4 out_4;
uniform sampler2D textureMap;   //Texture data array

uniform sampler2D tex0; //empty background texture
uniform sampler2D tex1; //red designated texture from alphatex0
uniform sampler2D tex2; //green designated texture from alphatex0
uniform sampler2D tex3; //blue designated texture from alphatex0
uniform sampler2D alphatex0; //texture indicating which red, green, blue or background texture to choose

uniform int isLayeredTexture;
uniform int id;

void main(){

	vec4 vTexColor = vec4(0.0, 0.0, 0.0, 1.0);
	
	if(isLayeredTexture == 1){
		vec4 backgroundTexture = texture2D(tex0, vsData.textureCoordinateOut.st*150);   
		vec4 red1 = texture2D(tex1, vsData.textureCoordinateOut.st*100);   
		vec4 green1 = texture2D(tex2, vsData.textureCoordinateOut.st*150);  
		vec4 blue1 = texture2D(tex3, vsData.textureCoordinateOut.st*150);   
		vec4 alpha1 = texture2D(alphatex0, vsData.textureCoordinateOut.st);       //alpha map 0
		
		//Used to prevent tiling
		vec4 tRandoBackground = texture2D(tex0, vsData.textureCoordinateOut.st*5.0);       //backgroundTexture
		vec4 tRandoR1 = texture2D(tex1, vsData.textureCoordinateOut.st*5.0);      			//red1
		vec4 tRandoG1 = texture2D(tex2, vsData.textureCoordinateOut.st*5.0);      			//green1
		vec4 tRandoB1 = texture2D(tex3, vsData.textureCoordinateOut.st*5.0);      			//blue1
		
		vec4 tBackGround1 = backgroundTexture * tRandoBackground;
		vec4 tR1 = red1 * tRandoR1;
		vec4 tG1 = green1 * tRandoG1;
		vec4 tB1 = blue1 * tRandoB1;
		
		float redVal1 = alpha1.r;
		float greenVal1 = alpha1.g;
		float blueVal1 = alpha1.b;
		
		//Start with grass as base lerpComponent
		vec4 lerpComponent = tBackGround1;
		//Path next
		lerpComponent = redVal1*tR1 + (1.0f - redVal1)*lerpComponent;
		//Mountain next
		lerpComponent = greenVal1*tG1 + (1.0f - greenVal1)*lerpComponent;
		//Dirt next
		lerpComponent = blueVal1*tB1 + (1.0f - blueVal1)*lerpComponent;
		
		vTexColor = vec4(lerpComponent.rgb, 1.0);
	}
	else{
		vTexColor = vec4(texture(textureMap, vsData.textureCoordinateOut).rgb, 1.0);
	}
	
	//Velocity buffer
	vec2 currProjPos = (vsData.projPositionOut.xy / vsData.projPositionOut.w);
	vec2 prevProjPos = (vsData.prevProjPositionOut.xy / vsData.prevProjPositionOut.w);

	out_1 = vTexColor;
	out_2 = vec4(normalize(vsData.normalOut), 1.0);
	out_3 = vec4(vec2(currProjPos - prevProjPos), 0.0, 1.0);
	out_4 = vec4(vec3((float(id)) / 255.0), 1.0);
}