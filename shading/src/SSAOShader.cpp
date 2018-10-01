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
    }
    else {
        _shader = new HLSLShader("ssaoShader");
    }
    glGenVertexArrays(1, &_dummyVAO);
}

SSAOShader::~SSAOShader() {

}

void SSAOShader::runShader(SSAO* ssao, MRTFrameBuffer* mrtBuffer, ViewEventDistributor* viewEventDistributor) {

    //LOAD IN SHADER
    _shader->bind();

    glBindVertexArray(_dummyVAO);

    _shader->updateData("projection", viewEventDistributor->getProjection().getFlatBuffer());
    _shader->updateData("projectionToViewMatrix", viewEventDistributor->getProjection().inverse().getFlatBuffer());
    
    auto kernel = ssao->getKernel();

    float* kernelArray = new float[3 * kernel.size()];
    int kernelIndex = 0;
    for (auto& vec : kernel) {
        float* kernelBuff = vec.getFlatBuffer();
        for (int i = 0; i < 3; i++) {
            kernelArray[kernelIndex++] = kernelBuff[i];
        }
    }

    _shader->updateData("kernel[0]", kernelArray);
    delete[] kernelArray;

    auto textures = mrtBuffer->getTextures();

    _shader->updateData("normalTexture",   GL_TEXTURE1, &textures[1]);
    //_shader->updateData("noiseTexture",    GL_TEXTURE2, ssao->getNoiseTexture());
    _shader->updateData("depthTexture",    GL_TEXTURE3, &textures[3]);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)4);

    glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture
    glBindVertexArray(0);
    glUseProgram(0);//end using this shader
}
