#version 330

layout(location = 0) in vec3 vertexIn; // Each vertex supplied
layout(location = 1) in vec3 normalIn; // Each normal supplied

out vec3 normalOut;
out vec3 texOut;
out vec3 positionOut;
out vec2 texCoordOut;

uniform mat4 model;      // Model and World transformation matrix
uniform mat4 view;       // View/Camera transformation matrix
uniform mat4 projection; // Projection transformation matrix
uniform mat4 normal;     // Normal matrix

void main(){
    vec4 posWorld = vec4(vertexIn.xyz, 1.0);

    positionOut = vec3(view * model * vec4(vertexIn.xyz, 1.0));
    texOut      = vec3(posWorld.x, posWorld.y, posWorld.z);
	
	float mapWidth = 16.0;
	float mapLength = 35.0;
	//transform position to uv indexing
	float u = (texOut.x + mapWidth/2.0)/mapWidth;
	float v = (texOut.z + mapLength/2.0)/mapLength;
	texCoordOut = vec2(u, v);
	
    normalOut   = vec3(normal * vec4(normalIn, 0.0));
    gl_Position = projection * view * model * vec4(vertexIn.xyz, 1.0);;
}
