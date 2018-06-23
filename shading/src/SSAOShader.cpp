#include "SSAOShader.h"
#include "SSAO.h"
#include "GLIncludes.h"
#include "MRTFrameBuffer.h"
#include "MVP.h"

SSAOShader::SSAOShader() : Shader("ssaoShader") {

    _positionTextureLocation = glGetUniformLocation(_shaderContext, "positionTexture");
    _normalTextureLocation = glGetUniformLocation(_shaderContext, "normalTexture");
    _noiseTextureLocation = glGetUniformLocation(_shaderContext, "noiseTexture");
    _kernelLocation = glGetUniformLocation(_shaderContext, "kernel");
    _projectionLocation = glGetUniformLocation(_shaderContext, "projection");

    glGenVertexArrays(1, &_dummyVAO);
}

SSAOShader::~SSAOShader() {

}

void SSAOShader::runShader(SSAO* ssao, MRTFrameBuffer* mrtBuffer, ViewManager* viewManager) {

    //LOAD IN SHADER
    glUseProgram(_shaderContext); //use context for loaded shader

    glBindVertexArray(_dummyVAO);

    //glUniform mat4 projection matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_projectionLocation, 1, GL_TRUE, viewManager->getProjection().getFlatBuffer());

    auto kernel = ssao->getKernel();

    float* kernelArray = new float[3 * kernel.size()];
    int kernelIndex = 0;
    for (auto& vec : kernel) {
        float* kernelBuff = vec.getFlatBuffer();
        for (int i = 0; i < 3; i++) {
            kernelArray[kernelIndex++] = kernelBuff[i];
        }
    }

    glUniform3fv(_kernelLocation, static_cast<GLsizei>(kernel.size()), kernelArray);
    delete[] kernelArray;

    auto textures = mrtBuffer->getTextureContexts();

    glUniform1i(_positionTextureLocation, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[2]);

    glUniform1i(_normalTextureLocation, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);

    glUniform1i(_noiseTextureLocation, 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, ssao->getNoiseTexture());

    glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)4);

    glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture
    glBindVertexArray(0);
    glUseProgram(0);//end using this shader
}
