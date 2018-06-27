#include "ComputeShader.h"
#include "SSAO.h"
#include "Uniforms.h"


ComputeShader::ComputeShader(std::string computeShaderName) : Shader(computeShaderName) {

    _readTextureLocation = glGetUniformLocation(_shaderContext, "readTexture");
    _writeTextureLocation = glGetUniformLocation(_shaderContext, "writeTexture");
}

ComputeShader::~ComputeShader() {

}

void ComputeShader::runShader(GLuint writeTexture, GLuint readTexture, Format format) {

    glUseProgram(_shaderContext);

    ImageData imageInfo = {};

    //Bind read textures
    imageInfo.readOnly = true;
    if (format == Format::RGBUB || format == Format::RGBF) {
        imageInfo.format = GL_RGBA8;
    }
    else if (format == Format::RF || format == Format::RU) {
        imageInfo.format = GL_R8;
    }
    updateUniform("readTexture", GL_TEXTURE0, readTexture, imageInfo);

    //Bind write textures
    imageInfo.readOnly = false;
    if (format == Format::RGBUB || format == Format::RGBF) {
        imageInfo.format = GL_RGBA8;
    }
    else if (format == Format::RF || format == Format::RU) {
        imageInfo.format = GL_R8;
    }
    updateUniform("writeTexture", GL_TEXTURE1, writeTexture, imageInfo);

    //Dispatch the shader
    glDispatchCompute(static_cast<GLuint>(ceilf(static_cast<float>(screenPixelWidth) / 16.0f)),
        static_cast<GLuint>(ceilf(static_cast<float>(screenPixelHeight) / 16.0f)), 1);

    glUseProgram(0);
}
