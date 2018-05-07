#version 330

in vec3 texCoords;

uniform samplerCube skybox;

void main()
{    
    gl_FragColor = texture(skybox, texCoords);
}