#version 330

in vec3 vertexIn;			   // Each vertex supplied 
in vec3 normalIn;			   // Each normal supplied 
in vec2 textureCoordinateIn;   // Each texture coordinate supplied
out vec3 normalOut;			   // Transformed normal based on the normal matrix transform
out vec3 directionalLight;     // Negated directional light to be used in fragment shader for dot product testing
out vec2 textureCoordinateOut; // Passthrough

uniform mat4 model;		 // Model and World transformation matrix
uniform mat4 view;		 // View/Camera transformation matrix
uniform mat4 projection; // Projection transformation matrix
uniform mat4 normal;     // Normal matrix

void main(){
	// The vertex is first transformed by the model and world, then 
	// the view/camera and finally the projection matrix
	// The order in which transformation matrices affect the vertex
	// is in the order from right to left
	vec4 transformedVert = projection * view * model * vec4(vertexIn.xyz, 1.0); 

	normalOut = vec3((normal * vec4(normalIn, 1.0)).xyz); //Transform normal coordinate in with the normal matrix
	
	textureCoordinateOut = textureCoordinateIn; //Passthrough
	
	//Fake directional light test pointing in the -x direction
	vec4 light = vec4(-1.0, 0.0, 0.0, 1.0); //Directional light vector
	vec3 transformedLight = vec3((normal * light).xyz); //Transform light vector with normal matrix
	directionalLight = vec3(-transformedLight.x, -transformedLight.y, -transformedLight.z); //negate the directional light for normal computations
	
	//Pass the transformed vertex to the fragment shader
	gl_Position = transformedVert; 
}