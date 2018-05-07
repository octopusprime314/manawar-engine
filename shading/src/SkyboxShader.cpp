#include "SkyboxShader.h"
#include "Model.h"
#include "SkyBox.h"

SkyboxShader::SkyboxShader(std::string shaderName) : Shader(shaderName) {

    //glUniform mat4 view matrix
    _viewLocation = glGetUniformLocation(_shaderContext, "view");

    //glUniform mat4 projection matrix
    _projectionLocation = glGetUniformLocation(_shaderContext, "projection");

    //glUniform texture cube map sampler location
    _cubeMapLocation = glGetUniformLocation(_shaderContext, "skybox");
}

SkyboxShader::~SkyboxShader() {

}

void SkyboxShader::runShader(SkyBox* skybox) {

    //LOAD IN SHADER
    glUseProgram(_shaderContext); //use context for loaded shader

    //LOAD IN VBO BUFFERS 
    VBO* vbo = skybox->getVBO();
    //Bind vertex buff context to current buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo->getVertexContext());

    //Say that the vertex data is associated with attribute 0 in the context of a shader program
    //Each vertex contains 3 floats per vertex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //Now enable vertex buffer at location 0
    glEnableVertexAttribArray(0);

    MVP* mvp = skybox->getMVP();

    //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_viewLocation, 1, GL_TRUE, mvp->getViewBuffer());

    //glUniform mat4 projection matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_projectionLocation, 1, GL_TRUE, mvp->getProjectionBuffer());

    glUniform1i(_cubeMapLocation, 0);

    //Diffuse texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, skybox->getCubeMapTexture()->getContext());

    glDisableVertexAttribArray(0); //Disable vertex attribute
    glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind buffer
    glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture
    glUseProgram(0);//end using this shader
}