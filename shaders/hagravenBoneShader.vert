#version 330

in vec3 vertexIn;			   // Each vertex supplied 
in vec3 normalIn;			   // Each normal supplied 
in vec2 textureCoordinateIn;   // Each texture coordinate supplied
in vec4 indexes;   // Maximum of 4 bones can affect one vertex
in vec4 weights;   // Maximum of 4 bones can affect one vertex

out vec3 normalOut;			   // Transformed normal based on the normal matrix transform
out vec3 directionalLight;     // Negated directional light to be used in fragment shader for dot product testing
out vec2 textureCoordinateOut; // Passthrough

uniform mat4 model;		 // Model and World transformation matrix
uniform mat4 view;		 // View/Camera transformation matrix
uniform mat4 projection; // Projection transformation matrix
uniform mat4 normal;     // Normal matrix

uniform mat4 bones[78]; // 78 bones

void main(){

	vec4 plainVert = vec4(vertexIn.xyz, 1.0);
	vec4 plainNorm = vec4(normalIn.xyz, 0.0);
	vec4 newVertex = (bones[int(indexes.x)] * weights.x) * plainVert + ((bones[int(indexes.y)] * weights.y) * plainVert) + ((bones[int(indexes.z)] * weights.z) * plainVert) + ((bones[int(indexes.w)] * weights.w) * plainVert);
	vec4 newNormal = (bones[int(indexes.x)] * weights.x) * plainNorm + ((bones[int(indexes.y)] * weights.y) * plainNorm) + ((bones[int(indexes.z)] * weights.z) * plainNorm) + ((bones[int(indexes.w)] * weights.w) * plainNorm);
	
	// The vertex is first transformed by the model and world, then 
	// the view/camera and finally the projection matrix
	// The order in which transformation matrices affect the vertex
	// is in the order from right to left
	vec4 transformedVert = projection * view * model * vec4(newVertex.xyz, 1.0); 

	normalOut = vec3((normal * vec4(newNormal.xyz, 1.0)).xyz); //Transform normal coordinate in with the normal matrix
	
	textureCoordinateOut = textureCoordinateIn; //Passthrough
	
	//Fake directional light test pointing in the -x direction
	vec4 light = vec4(-1.0, 0.0, 0.0, 1.0); //Directional light vector
	vec3 transformedLight = vec3((normal * light).xyz); //Transform light vector with normal matrix
	directionalLight = vec3(-transformedLight.x, -transformedLight.y, -transformedLight.z); //negate the directional light for normal computations
	
	//Pass the transformed vertex to the fragment shader
	gl_Position = transformedVert; 
}