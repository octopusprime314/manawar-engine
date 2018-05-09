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

    MVP* mvp = skybox->getMVP();

    //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_viewLocation, 1, GL_TRUE, mvp->getViewMatrix().inverse().getFlatBuffer());

    //glUniform mat4 projection matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_projectionLocation, 1, GL_TRUE, mvp->getProjectionMatrix().inverse().getFlatBuffer());

    //Skybox texture
    glUniform1i(_cubeMapLocation, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->getCubeMapTexture()->getContext());

    //Draw screen space triangles for skybox
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0); //Unbind texture
    glUseProgram(0);//end using this shader
}