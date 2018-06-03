#version 330
layout(location = 0) in vec3 vertexIn;			   // Each vertex supplied 
out vec2 texCoord;
out vec3 position;
uniform mat4 mv; // Model view matrix 
uniform mat4 p;  //projection matrix
//uniform mat4 inverseViewNoTrans;

void main()
{
	if(vertexIn.x == -1.0 && vertexIn.y == 1.0) {
		texCoord = vec2(0.0, 1.0);
	}
	else if(vertexIn.x == -1.0 && vertexIn.y == -1.0) {
		texCoord = vec2(0.0, 0.0);
	}
	else if(vertexIn.x == 1.0 && vertexIn.y == 1.0) {
		texCoord = vec2(1.0, 1.0);
	}
	else if(vertexIn.x == 1.0 && vertexIn.y == -1.0) {
		texCoord = vec2(1.0, 0.0);
	}

	// The vertex is first transformed by the model and world, then 
	// the view/camera and finally the projection matrix
	// The order in which transformation matrices affect the vertex
	// is in the order from right to left
	vec4 transformedVert = p * mv * vec4(vertexIn.xyz, 1.0); 
	gl_Position = transformedVert; 
	
	position = vec3((mv * vec4(vertexIn.xyz, 1.0)).xyz); 
}