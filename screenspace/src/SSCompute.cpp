#include "SSCompute.h"
#include "GLIncludes.h"
#include "ShaderBroker.h"

SSCompute::SSCompute(std::string computeShader, GLuint width, GLuint height, TextureFormat format) :
    _renderTexture(width, height, format),
    _computeShader(static_cast<ComputeShader*>(ShaderBroker::instance()->getShader(computeShader))) {

    _format = format;
}

SSCompute::~SSCompute() {

}

unsigned int SSCompute::getTextureContext() {
    return _renderTexture.getContext();
}

Texture* SSCompute::getTexture() {
    return &_renderTexture;
}


void SSCompute::compute(Texture* readTexture) {
    _computeShader->runShader(&_renderTexture, readTexture, _format);
}

void SSCompute::compute(Texture* readTexture, Texture* writeTexture) {
    _computeShader->runShader(writeTexture, readTexture, _format);
}