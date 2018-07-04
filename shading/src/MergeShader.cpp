#include "MergeShader.h"
#include "GLIncludes.h"
#include "MRTFrameBuffer.h"
#include "MVP.h"

MergeShader::MergeShader() : Shader("mergeShader") {
    glGenVertexArrays(1, &_dummyVAO);
}

MergeShader::~MergeShader() {

}

void MergeShader::runShader(GLuint deferredTexture, GLuint velocityTexture) {

    //LOAD IN SHADER
    glUseProgram(_shaderContext); //use context for loaded shader
    glBindVertexArray(_dummyVAO);

    updateUniform("deferredTexture", GL_TEXTURE0, deferredTexture);
    updateUniform("velocityTexture", GL_TEXTURE1, velocityTexture);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)4);

    glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture
    glBindVertexArray(0);
    glUseProgram(0);//end using this shader
}
