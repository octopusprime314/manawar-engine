#include "ShadowPointShader.h"
#include "Entity.h"
#include "GLSLShader.h"
#include "HLSLShader.h"
#include "EngineManager.h"
#include "GLSLShader.h"
#include "HLSLShader.h"
#include "EngineManager.h"

ShadowPointShader::ShadowPointShader(std::string shaderName) {
    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        _shader = new GLSLShader(shaderName);
    }
    else {
        _shader = new HLSLShader(shaderName);
    }
}

ShadowPointShader::~ShadowPointShader() {

}

void ShadowPointShader::runShader(Entity*             entity,
                                  Light*              light,
                                  std::vector<Matrix> lightTransforms) {

    //Load in vbo buffers
    auto model             = entity->getModel();
    std::vector<VAO*>* vao = model->getVAO();
    MVP* modelMVP          = entity->getMVP();
    MVP lightMVP           = light->getLightMVP();

    //Use one single shadow shader and replace the vbo buffer from each model
    _shader->bind();
    
    for (auto vaoInstance : *vao) {
        glBindVertexArray(vaoInstance->getVAOShadowContext());

        _shader->updateData("model", modelMVP->getModelBuffer());

        float* lightCubeTransforms = new float[6 * 16];
        int index = 0;
        for (Matrix lightTransform : lightTransforms) {
            float* mat = lightTransform.getFlatBuffer();
            for (int i = 0; i < 16; ++i) {
                lightCubeTransforms[index++] = mat[i];
            }
        }

        //6 faces and each transform is 16 floats in a 4x4 matrix
        _shader->updateData("shadowMatrices[0]", lightCubeTransforms);
        delete[] lightCubeTransforms;

        //Set light position for point light
        auto lightPos = light->getPosition().getFlatBuffer();
        _shader->updateData("lightPos", lightPos);

        //Set far plane for depth scaling
        //Quick trick to get far value out of projection matrix
        auto projMatrix = lightMVP.getProjectionBuffer();
        float nearVal   = (2.0f*projMatrix[11]) / (2.0f*projMatrix[10] - 2.0f);
        float farVal    = ((projMatrix[10] - 1.0f)*nearVal) / (projMatrix[10] + 1.0f);
        _shader->updateData("farPlane", &farVal);

        auto textureStrides = vaoInstance->getTextureStrides();
        unsigned int verticesSize = 0;
        for (auto textureStride : textureStrides) {
            verticesSize += textureStride.second;
        }

        //Draw triangles using the bound buffer vertices at starting index 0 and number of vertices
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)verticesSize);

        glBindVertexArray(0);
    }
    glUseProgram(0);//end using this shader
}
