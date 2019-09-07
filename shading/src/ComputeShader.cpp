#include "ComputeShader.h"
#include "SSAO.h"
#include "Uniforms.h"
#include "GLSLShader.h"
#include "HLSLShader.h"
#include "EngineManager.h"

ComputeShader::ComputeShader(std::string computeShaderName) {
    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        _shader = new GLSLShader(computeShaderName);
    }
    else {
        std::vector<DXGI_FORMAT>* formats = new std::vector<DXGI_FORMAT>();
        formats->push_back(DXGI_FORMAT_R32_FLOAT);
        _shader = new HLSLShader(computeShaderName, "", formats);
    }
}

ComputeShader::~ComputeShader() {

}

void ComputeShader::runShader(Texture*      writeTexture,
                              Texture*      readTexture,
                              TextureFormat format) {

    _shader->bind();
    ImageData imageInfo = {};
    //Bind read textures
    imageInfo.readOnly  = true;
    if (format == TextureFormat::RGBA_UNSIGNED_BYTE ||
        format == TextureFormat::RGBA_FLOAT) {
        imageInfo.format = GL_RGBA8;
    }
    else if (format == TextureFormat::R_FLOAT ||
             format == TextureFormat::R_UNSIGNED_BYTE) {
        imageInfo.format = GL_R8;
    }
    _shader->updateData("readTexture",  GL_TEXTURE0, readTexture,  imageInfo);
    imageInfo.readOnly = false;
    _shader->updateData("writeTexture", GL_TEXTURE1, writeTexture, imageInfo);

    //Dispatch the shader
    _shader->dispatch(ceilf(static_cast<float>(writeTexture->getWidth())  / 16.0f),
                      ceilf(static_cast<float>(writeTexture->getHeight()) / 16.0f),
                      1);
    _shader->unbind();
}
