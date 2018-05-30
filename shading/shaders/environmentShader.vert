#version 330

layout(location = 0) in vec3 vertexIn;			   // Each vertex supplied 
layout(location = 1) in vec3 normalIn;			   // Each normal supplied 
layout(location = 2) in vec2 textureCoordinateIn;   // Each texture coordinate supplied

uniform mat4 mv;		 // Model and World transformation matrix
out VsData
{
	vec3 normal;
    vec2 textureCoord;
}  vsData;

void main(){
	// The vertex is first transformed by the model and world, then 
	// the view/camera and finally the projection matrix
	// The order in which transformation matrices affect the vertex
	// is in the order from right to left
	vec4 transformedVert = mv * vec4(vertexIn.xyz, 1.0); 
	
	vsData.textureCoord = textureCoordinateIn; //Passthrough
	vsData.normal = normalIn; //Passthrough
	
	//Pass the transformed vertex to the fragment shader
	gl_Position = transformedVert; 
}