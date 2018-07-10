#include "SSAOShader.h"
#include "SSAO.h"
#include "GLIncludes.h"
#include "MRTFrameBuffer.h"
#include "MVP.h"

SSAOShader::SSAOShader() : Shader("ssaoShader") {
    glGenVertexArrays(1, &_dummyVAO);
}

SSAOShader::~SSAOShader() {

}

void SSAOShader::runShader(SSAO* ssao, MRTFrameBuffer* mrtBuffer, ViewManager* viewManager) {

    //LOAD IN SHADER
    glUseProgram(_shaderContext); //use context for loaded shader

    glBindVertexArray(_dummyVAO);

    updateUniform("projection", viewManager->getProjection().getFlatBuffer());
    updateUniform("projectionToViewMatrix", viewManager->getProjection().inverse().getFlatBuffer());
    
    auto kernel = ssao->getKernel();

    float* kernelArray = new float[3 * kernel.size()];
    int kernelIndex = 0;
    for (auto& vec : kernel) {
        float* kernelBuff = vec.getFlatBuffer();
        for (int i = 0; i < 3; i++) {
            kernelArray[kernelIndex++] = kernelBuff[i];
        }
    }

    updateUniform("kernel[0]", kernelArray);
    delete[] kernelArray;

    auto textures = mrtBuffer->getTextureContexts();

    updateUniform("normalTexture",   GL_TEXTURE1, textures[1]);
    updateUniform("noiseTexture",    GL_TEXTURE2, ssao->getNoiseTexture());
    updateUniform("depthTexture",    GL_TEXTURE3, textures[3]);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)4);

    glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture
    glBindVertexArray(0);
    glUseProgram(0);//end using this shader
}
