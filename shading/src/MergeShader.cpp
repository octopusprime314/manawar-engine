#include "MergeShader.h"
#include "GLIncludes.h"
#include "MRTFrameBuffer.h"
#include "MVP.h"

MergeShader::MergeShader() : Shader("mergeShader") {
    glGenVertexArrays(1, &_dummyVAO);
}

MergeShader::~MergeShader() {

}

void MergeShader::runShader(GLuint deferredTexture, GLuint bloomTexture) {

    //LOAD IN SHADER
    glUseProgram(_shaderContext); //use context for loaded shader
    glBindVertexArray(_dummyVAO);

    updateUniform("deferredTexture", GL_TEXTURE0, deferredTexture, GL_TEXTURE_2D);
    updateUniform("bloomTexture", GL_TEXTURE1, bloomTexture, GL_TEXTURE_2D);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)4);

    glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture
    glBindVertexArray(0);
    glUseProgram(0);//end using this shader
}