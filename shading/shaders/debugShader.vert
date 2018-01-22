#version 330

layout(location = 0) in vec3 vertexIn;			 // Each vertex supplied 

uniform mat4 model;		 // Model and World transformation matrix
uniform mat4 view;		 // View/Camera transformation matrix
uniform mat4 projection; // Projection transformation matrix

void main(){
	// The vertex is first transformed by the model and world, then 
	// the view/camera and finally the projection matrix
	// The order in which transformation matrices affect the vertex
	// is in the order from right to left
	vec4 transformedVert = projection * view * model * vec4(vertexIn.xyz, 1.0); 

	//Pass the transformed vertex to the fragment shader
	gl_Position = transformedVert; 
}