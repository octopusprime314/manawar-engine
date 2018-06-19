#include "MergeShader.h"
#include "GLIncludes.h"
#include "MRTFrameBuffer.h"
#include "MVP.h"

MergeShader::MergeShader() : Shader("mergeShader") {

    _deferredTextureLocation = glGetUniformLocation(_shaderContext, "deferredTexture");
    _bloomTextureLocation = glGetUniformLocation(_shaderContext, "bloomTexture");
    glGenVertexArrays(1, &_dummyVAO);
}

MergeShader::~MergeShader() {

}

void MergeShader::runShader(GLuint deferredTexture, GLuint bloomTexture) {

    //LOAD IN SHADER
    glUseProgram(_shaderContext); //use context for loaded shader

    glBindVertexArray(_dummyVAO);

    glUniform1i(_deferredTextureLocation, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, deferredTexture);

    glUniform1i(_deferredTextureLocation, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bloomTexture);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)4);

    glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture
    glBindVertexArray(0);
    glUseProgram(0);//end using this shader
}