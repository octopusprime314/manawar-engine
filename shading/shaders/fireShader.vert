#version 330

out vec2 texCoord;
out vec3 position;
uniform int fireType;
uniform mat4 mvp;  // Model view and projection matrix

void main()
{
	
	//there are five possible fire types in the pixel shader
	//choosing which fire type is in intervals of 0.2 in the u coordinate
	//and the v coordinates will always be between 0 and 0.5 for now
	float minU = fireType * 0.2f;
	float maxU  = minU + 0.2f;
	
	if(gl_VertexID == 0) {
		gl_Position = mvp * vec4(-1.0, 1.0, 0.0, 1.0);
		texCoord = vec2(minU, 0.5);
	}
	else if(gl_VertexID == 1) {
		gl_Position = mvp * vec4(-1, -1, 0, 1);
		texCoord = vec2(minU, 0.0);
	}
	else if(gl_VertexID == 2) {
		gl_Position = mvp * vec4(1, 1, 0, 1);
		texCoord = vec2(maxU, 0.5);
	}
	else if(gl_VertexID == 3) {
		gl_Position = mvp * vec4(1, -1, 0, 1);
		texCoord = vec2(maxU, 0.0);
	}
	position = gl_Position.xyz;
}