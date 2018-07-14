#version 430
uniform vec3 color;
out vec4 fragColor;
void main(){

	fragColor = vec4(color, 0.0); //Make this 0.2 to see frustums in the scene including shadows and cameras
}