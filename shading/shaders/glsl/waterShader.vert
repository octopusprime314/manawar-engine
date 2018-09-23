#version 430

out VsData
{
	vec2 texCoordOut;
	vec3 positionOut;
} vsData;

uniform mat4 model; // Model view matrix 
uniform mat4 view; // View matrix 
uniform mat4 projection;  //projection matrix

void main()
{
	vec3 vertex = vec3(0.0);
	if(gl_VertexID == 0) {
		vertex      	   = vec3(-1.0, 1.0, 0.0);  
		vsData.texCoordOut = vec2( 0.0, 1.0);
	}
	else if(gl_VertexID == 1) {
		vertex      	   = vec3(-1.0, -1.0, 0.0);  
		vsData.texCoordOut = vec2( 0.0,  0.0);
	}
	else if(gl_VertexID == 2) {
		vertex      	   = vec3(1.0, 1.0, 0.0);  
		vsData.texCoordOut = vec2(1.0, 1.0);
	}
	else if(gl_VertexID == 3) {
		vertex             = vec3(1.0, -1.0, 0.0);  
		vsData.texCoordOut = vec2(1.0,  0.0);
	}

	// The vertex is first transformed by the model and world, then 
	// the view/camera and finally the projection matrix
	// The order in which transformation matrices affect the vertex
	// is in the order from right to left
	vec4 transformedVert = projection * view * model * vec4(vertex.xyz, 1.0); 
	gl_Position = transformedVert; 
	
	vsData.positionOut = vec3((view * model * vec4(vertex.xyz, 1.0)).xyz); 
}