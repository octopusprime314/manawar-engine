#version 330

layout(location = 0) in vec3 vertexIn;			   // Each vertex supplied 

uniform mat4 view;		 // View/Camera transformation matrix
uniform mat4 projection; // Projection transformation matrix

out vec3 texCoords;

void main(){

	texCoords = vertexIn.xyz;
	
	// The vertex is first transformed by 
	// the view/camera and finally the projection matrix
	// The order in which transformation matrices affect the vertex
	// is in the order from right to left
	vec4 transformedVert = projection * view * vec4(vertexIn.xyz, 1.0); 
	
	//Pass the transformed vertex to the fragment shader
	gl_Position = transformedVert; 
}