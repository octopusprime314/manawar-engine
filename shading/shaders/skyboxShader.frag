#version 330

in vec3 vsViewDirection;
 
uniform samplerCube skybox;
 
void main() {
    gl_FragColor = texture(skybox, vec3(vsViewDirection.x, -vsViewDirection.y, vsViewDirection.z));
}