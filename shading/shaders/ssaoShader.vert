#version 330

out vec2 texCoord;

void main()
{
	if(gl_VertexID == 0) {
		gl_Position = vec4(-1.0, 1.0, 0.0, 1.0);
		texCoord = vec2(0.0, 1.0);
	}
	else if(gl_VertexID == 1) {
		gl_Position = vec4(-1, -1, 0, 1);
		texCoord = vec2(0.0, 0.0);
	}
	else if(gl_VertexID == 2) {
		gl_Position = vec4(1, 1, 0, 1);
		texCoord = vec2(1.0, 1.0);
	}
	else if(gl_VertexID == 3) {
		gl_Position = vec4(1, -1, 0, 1);
		texCoord = vec2(1.0, 0.0);
	}
}