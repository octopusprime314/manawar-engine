#include "ComputeShader.h"
#include "SSAO.h"
#include "Uniforms.h"


ComputeShader::ComputeShader(std::string computeShaderName) : Shader(computeShaderName) {

}

ComputeShader::~ComputeShader() {

}

void ComputeShader::runShader(Texture* writeTexture, Texture* readTexture, TextureFormat format) {

    glUseProgram(_shaderContext);

    ImageData imageInfo = {};

    //Bind read textures
    imageInfo.readOnly = true;
    if (format == TextureFormat::RGBA_UNSIGNED_BYTE || format == TextureFormat::RGBA_FLOAT) {
        imageInfo.format = GL_RGBA8;
    }
    else if (format == TextureFormat::R_FLOAT || format == TextureFormat::R_UNSIGNED_BYTE) {
        imageInfo.format = GL_R8;
    }
    updateUniform("readTexture", GL_TEXTURE0, readTexture->getContext(), imageInfo);

    imageInfo.readOnly = false;
    updateUniform("writeTexture", GL_TEXTURE1, writeTexture->getContext(), imageInfo);

    //Dispatch the shader
    glDispatchCompute(static_cast<GLuint>(ceilf(static_cast<float>(writeTexture->getWidth()) / 16.0f)),
        static_cast<GLuint>(ceilf(static_cast<float>(writeTexture->getHeight()) / 16.0f)), 1);

    glUseProgram(0);
}
