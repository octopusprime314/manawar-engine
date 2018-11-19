#include "SSAO.h"
#include <random>
#include "GLIncludes.h"
#include "MRTFrameBuffer.h"
#include "ViewEventDistributor.h"
#include "ShaderBroker.h"
#include "DXLayer.h"
#include "EngineManager.h"
#include "HLSLShader.h"

SSAO::SSAO() :
    _renderTexture(IOEventDistributor::screenPixelWidth, IOEventDistributor::screenPixelHeight, TextureFormat::R_FLOAT),
    _ssaoShader(static_cast<SSAOShader*>(ShaderBroker::instance()->getShader("ssaoShader"))) {

    //_blur = new SSCompute("blurShader", IOEventDistributor::screenPixelWidth / 4, IOEventDistributor::screenPixelHeight / 4, TextureFormat::R_FLOAT);
    _downSample = new SSCompute("downsample", IOEventDistributor::screenPixelWidth / 4, IOEventDistributor::screenPixelHeight / 4, TextureFormat::R_FLOAT);
    //_upSample = new SSCompute("upsample", IOEventDistributor::screenPixelWidth, IOEventDistributor::screenPixelHeight, TextureFormat::R_FLOAT);

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        glGenFramebuffers(1, &_ssaoFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, _ssaoFBO);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _renderTexture.getContext(), 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
   
    _generateKernelNoise();
}

SSAO::~SSAO() {

}

//helper lerp, move to math function utility someday
float lerp(float a, float b, float f) {
    return a + f * (b - a);
}

void SSAO::_generateKernelNoise() {
    // random floats between 0.0 - 1.0
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
    std::default_random_engine generator;

    for (unsigned int i = 0; i < 64; ++i) {
        Vector4 sample(
            randomFloats(generator) * 2.0f - 1.0f,
            randomFloats(generator) * 2.0f - 1.0f,
            randomFloats(generator),
            1.0f
        );
        sample.normalize();
        sample = sample * randomFloats(generator);
        float scale = static_cast<float>(i) / 64.0f;

        scale = lerp(0.1f, 1.0f, scale * scale);
        sample = sample * scale;
        _ssaoKernel.push_back(sample);
    }

    for (unsigned int i = 0; i < 16; i++) {
        Vector4 noise(
            randomFloats(generator) * 2.0f - 1.0f,
            randomFloats(generator) * 2.0f - 1.0f,
            0.0f,
            1.0f);
        _ssaoNoise.push_back(noise);
    }

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::DX12) {
        _noise = new AssetTexture(&_ssaoNoise[0], 4, 4,
            DXLayer::instance()->getCmdList(), DXLayer::instance()->getDevice());
    }
    else {
        _noise = new AssetTexture(&_ssaoNoise[0], 4, 4);
    }
}

void SSAO::computeSSAO(MRTFrameBuffer* mrtBuffer, ViewEventDistributor* viewEventDistributor) {

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::DX12) {

        std::vector<RenderTexture> textures = { _renderTexture};
        HLSLShader::setOM(textures, IOEventDistributor::screenPixelWidth, IOEventDistributor::screenPixelHeight);
    }
    else {
        glBindFramebuffer(GL_FRAMEBUFFER, _ssaoFBO);

        glClear(GL_COLOR_BUFFER_BIT);
    }

    _ssaoShader->runShader(this, mrtBuffer, viewEventDistributor);

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::DX12) {
        std::vector<RenderTexture> textures = { _renderTexture };
        HLSLShader::releaseOM(textures);
    }
    else {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    DXLayer::instance()->getCmdList()->ResourceBarrier(1, 
        &CD3DX12_RESOURCE_BARRIER::Transition(_renderTexture.getResource()->getResource().Get(),
        D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_GENERIC_READ));

    //Downsample by a 1/4
    _downSample->compute(&_renderTexture);

    DXLayer::instance()->getCmdList()->ResourceBarrier(1,
        &CD3DX12_RESOURCE_BARRIER::Transition(_renderTexture.getResource()->getResource().Get(),
            D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COMMON));

    ////Blur in downsampled 
    //_blur->compute(_downSample->getTexture());

    ////upsample back to original
    //_upSample->compute(_blur->getTexture());
}

Texture* SSAO::getNoiseTexture() {
    return _noise;
}

std::vector<Vector4>& SSAO::getKernel() {
    return _ssaoKernel;
}

SSCompute* SSAO::getBlur() {
    return _upSample;
}

Texture* SSAO::getSSAOTexture() {
    return _downSample->getTexture();
}