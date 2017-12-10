#version 330

in vec3 vertexIn;			   // Each vertex supplied 
in vec3 normalIn;			   // Each normal supplied 
in vec2 textureCoordinateIn;   // Each texture coordinate supplied
in float indexes[5];   // Maximum of 5 bones can affect one vertex
in float weights[5];   // Maximum of 5 bones can affect one vertex

out vec3 normalOut;			   // Transformed normal based on the normal matrix transform
out vec3 directionalLight;     // Negated directional light to be used in fragment shader for dot product testing
out vec2 textureCoordinateOut; // Passthrough

uniform mat4 model;		 // Model and World transformation matrix
uniform mat4 view;		 // View/Camera transformation matrix
uniform mat4 projection; // Projection transformation matrix
uniform mat4 normal;     // Normal matrix

uniform mat4 bones[103]; // 103 bones

void main(){

	vec4 plainVert = vec4(vertexIn.xyz, 1.0);
	vec4 plainNorm = vec4(normalIn.xyz, 0.0);
	vec4 newVertex = (bones[int(indexes[0])] * weights[0]) * plainVert + ((bones[int(indexes[1])] * weights[1]) * plainVert) + ((bones[int(indexes[2])] * weights[2]) * plainVert) + ((bones[int(indexes[3])] * weights[3]) * plainVert) + ((bones[int(indexes[4])] * weights[4]) * plainVert);
	vec4 newNormal = (bones[int(indexes[0])] * weights[0]) * plainNorm + ((bones[int(indexes[1])] * weights[1]) * plainNorm) + ((bones[int(indexes[2])] * weights[2]) * plainNorm) + ((bones[int(indexes[3])] * weights[3]) * plainNorm) + ((bones[int(indexes[4])] * weights[4]) * plainNorm);
	
	// The vertex is first transformed by the model and world, then 
	// the view/camera and finally the projection matrix
	// The order in which transformation matrices affect the vertex
	// is in the order from right to left
	vec4 transformedVert = projection * view * model * vec4(newVertex.xyz, 1.0); 

	normalOut = vec3((normal * vec4(newNormal.xyz, 1.0)).xyz); //Transform normal coordinate in with the normal matrix
	
	textureCoordinateOut = textureCoordinateIn; //Passthrough
	
	//Fake directional light test pointing in the -x direction
	vec4 light = vec4(0.0, 0.0, -1.0, 1.0); //Directional light vector
	vec3 transformedLight = vec3((normal * light).xyz); //Transform light vector with normal matrix
	directionalLight = vec3(-transformedLight.x, -transformedLight.y, -transformedLight.z); //negate the directional light for normal computations
	
	//Pass the transformed vertex to the fragment shader
	gl_Position = transformedVert; 
}