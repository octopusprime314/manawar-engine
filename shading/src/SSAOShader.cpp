#include "SSAOShader.h"
#include "SSAO.h"
#include "GLIncludes.h"
#include "MRTFrameBuffer.h"
#include "MVP.h"
#include "GLSLShader.h"
#include "HLSLShader.h"
#include "EngineManager.h"

SSAOShader::SSAOShader() {
    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        _shader = new GLSLShader("ssaoShader");
        glGenVertexArrays(1, &_dummyVAO);
    }
    else {
        std::vector<DXGI_FORMAT>* formats = new std::vector<DXGI_FORMAT>();
        formats->push_back(DXGI_FORMAT_R32_FLOAT);
        _shader = new HLSLShader("ssaoShader", "", formats);
    }
}

SSAOShader::~SSAOShader() {

}

void SSAOShader::runShader(SSAO* ssao, MRTFrameBuffer* mrtBuffer, ViewEventDistributor* viewEventDistributor) {

    //LOAD IN SHADER
    _shader->bind();

    if(EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        glBindVertexArray(_dummyVAO);
    }
    else {
        _shader->bindAttributes(nullptr);
    }

    _shader->updateData("projection", viewEventDistributor->getProjection().getFlatBuffer());
    _shader->updateData("projectionToViewMatrix", viewEventDistributor->getProjection().inverse().getFlatBuffer());
    
    auto kernel = ssao->getKernel();
    float* kernelArray = nullptr;
    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        kernelArray = new float[3 * kernel.size()];
        int kernelIndex = 0;
        for (auto& vec : kernel) {
            float* kernelBuff = vec.getFlatBuffer();
            for (int i = 0; i < 3; i++) {
                kernelArray[kernelIndex++] = kernelBuff[i];
            }
        }
    }
    else {
        kernelArray = new float[4 * kernel.size()];
        int kernelIndex = 0;
        for (auto& vec : kernel) {
            float* kernelBuff = vec.getFlatBuffer();
            for (int i = 0; i < 4; i++) {
                if (i < 3) {
                    kernelArray[kernelIndex] = kernelBuff[i];
                }
                kernelIndex++;
            }
        }
    }

    _shader->updateData("kernel[0]", kernelArray);
    delete[] kernelArray;

    auto textures = mrtBuffer->getTextures();

    _shader->updateData("normalTexture",   GL_TEXTURE1, &textures[1]);
    _shader->updateData("noiseTexture",    GL_TEXTURE2, ssao->getNoiseTexture());
    _shader->updateData("depthTexture",    GL_TEXTURE3, &textures[3]);

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)4);
    }
    else {
        _shader->draw(0, 1, 3);
    }

    _shader->unbindAttributes();
    _shader->unbind();
}
