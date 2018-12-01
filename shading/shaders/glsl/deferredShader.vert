#version 430
out VsData
{
	vec2 texCoordOut; 
	vec3 vsViewDirection;
} vsData;

uniform mat4 inverseView; // Inverse View/Camera transformation matrix
uniform mat4 inverseProjection; // Inverse Projection transformation matrix

void main() {

	vec2 vertex = vec2(0.0);
	if(gl_VertexID == 0) {
		vertex             = vec2(-1.0, 1.0);  
		vsData.texCoordOut = vec2( 0.0, 1.0);
	}
	else if(gl_VertexID == 1) {
		vertex             = vec2(-1.0, -1.0);  
		vsData.texCoordOut = vec2( 0.0,  0.0);
	}
	else if(gl_VertexID == 2) {
		vertex             = vec2(1.0, 1.0);  
		vsData.texCoordOut = vec2(1.0, 1.0);
	}
	else if(gl_VertexID == 3) {
		vertex             = vec2(1.0, -1.0);  
		vsData.texCoordOut = vec2(1.0,  0.0);
	}
	
	gl_Position = vec4(vertex.xy, 1.0, 1.0); 
	vsData.vsViewDirection = mat3(inverseView) * (inverseProjection * gl_Position * -1.0f).xyz;
	
}