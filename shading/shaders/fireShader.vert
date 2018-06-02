#version 330
layout(location = 0) in vec3 vertexIn;			   // Each vertex supplied 
out vec2 texCoord;
out vec3 position;
uniform int fireType;
uniform mat4 mv; // Model view matrix 
uniform mat4 p;  //projection matrix
uniform mat4 inverseViewNoTrans;

void main()
{
	
	//there are five possible fire types in the pixel shader
	//choosing which fire type is in intervals of 0.2 in the u coordinate
	//and the v coordinates will always be between 0 and 0.5 for now
	float minU = fireType * 0.2f;
	float maxU  = minU + 0.2f;
	
	if(vertexIn.x == -1.0 && vertexIn.y == 1.0) {
		texCoord = vec2(minU, 0.5);
	}
	else if(vertexIn.x == -1.0 && vertexIn.y == -1.0) {
		texCoord = vec2(minU, 0.0);
	}
	else if(vertexIn.x == 1.0 && vertexIn.y == 1.0) {
		texCoord = vec2(maxU, 0.5);
	}
	else if(vertexIn.x == 1.0 && vertexIn.y == -1.0) {
		texCoord = vec2(maxU, 0.0);
	}

	// The vertex is first transformed by the model and world, then 
	// the view/camera and finally the projection matrix
	// The order in which transformation matrices affect the vertex
	// is in the order from right to left
	vec4 transformedVert = p * mv * inverseViewNoTrans * vec4(vertexIn.xyz, 1.0); 
	gl_Position = transformedVert; 
	
	position = vec3((mv * inverseViewNoTrans * vec4(vertexIn.xyz, 1.0)).xyz); 
}