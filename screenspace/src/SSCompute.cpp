#include "SSCompute.h"
#include "GLIncludes.h"

SSCompute::SSCompute(std::string computeShader, GLuint width, GLuint height, TextureFormat format) :
    _renderTexture(width, height, format){

    _computeShader = new ComputeShader(computeShader);
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