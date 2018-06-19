#include "ComputeShader.h"
#include "Blur.h"
#include "SSAO.h"


ComputeShader::ComputeShader(std::string computeShaderName) : Shader(computeShaderName) {

    _readTextureLocation = glGetUniformLocation(_shaderContext, "readTexture");
    _writeTextureLocation = glGetUniformLocation(_shaderContext, "writeTexture");
}

ComputeShader::~ComputeShader() {

}

void ComputeShader::runShader(GLuint writeTexture, GLuint readTexture, bool rgb) {

    glUseProgram(_shaderContext);

    //Bind textures
    glUniform1i(_readTextureLocation, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, readTexture);

    //Bind color textures
    glUniform1i(_writeTextureLocation, 1);
    if (rgb) {
        glBindImageTexture(1, writeTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
    }
    else {
        glBindImageTexture(1, writeTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8);
    }
    //Dispatch the shader
    glDispatchCompute(static_cast<GLuint>(ceilf(static_cast<float>(screenPixelWidth) / 16.0f)),
        static_cast<GLuint>(ceilf(static_cast<float>(screenPixelHeight) / 16.0f)), 1);


    glUseProgram(0);
}
