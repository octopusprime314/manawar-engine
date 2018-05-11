#version 330

layout(location = 0) in vec3 vertexIn;
layout(location = 1) in vec2 textureCoordinateIn;
out vec2 textureCoordinateOut; // Passthrough
out vec3 vsViewDirection;
uniform mat4 inverseView;		 // View/Camera transformation matrix
uniform mat4 inverseProjection; // Projection transformation matrix

void main(){

	textureCoordinateOut = textureCoordinateIn; //Passthrough
	gl_Position = vec4(vertexIn.xyz, 1.0); //Passthrough
	vsViewDirection = mat3(inverseView) * (inverseProjection * gl_Position).xyz;
}