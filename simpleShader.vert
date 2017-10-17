#version 330

in vec3 vertex; // Each vertex supplied by glDrawArrays, etc.

uniform mat4 M; // Model and World transformation matrix
uniform mat4 V; // View/Camera transformation matrix
uniform mat4 P; // Projection transformation matrix

void main(){
	//vec3 transformedVert = P * V * M * vec4(vertex.xyz, 1.0); // The vertex is first transformed by the model and world, then 
	//														  // the view/camera and finally the projection matrix
	//														  // The order in which transformation matrices affect the vertex
	//														  // is in the order from right to left
	////Pass the transformed vertex to the fragment shader
	//gl_Position = transformedVert; 

	gl_Position = P * V * M * vec4(vertex.xyz, 1.0);
}