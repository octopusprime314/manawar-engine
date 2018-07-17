#include "ShadowPointShader.h"
#include "Entity.h"

ShadowPointShader::ShadowPointShader(std::string shaderName) : Shader(shaderName) {

}

ShadowPointShader::~ShadowPointShader() {

}

void ShadowPointShader::runShader(Entity* entity, Light* light, std::vector<Matrix> lightTransforms) {

    //Load in vbo buffers
    auto model = entity->getModel();
    std::vector<VAO*>* vao = model->getVAO();
    MVP* modelMVP = entity->getMVP();
    MVP lightMVP = light->getLightMVP();

    //Use one single shadow shader and replace the vbo buffer from each model
    glUseProgram(_shaderContext); //use context for loaded shader
    
    for (auto vaoInstance : *vao) {
        glBindVertexArray(vaoInstance->getVAOShadowContext());

        //glUniform mat4 combined model and world matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        updateUniform("model", modelMVP->getModelBuffer());

        float* lightCubeTransforms = new float[6 * 16];
        int index = 0;
        for (Matrix lightTransform : lightTransforms) {
            float* mat = lightTransform.getFlatBuffer();
            for (int i = 0; i < 16; ++i) {
                lightCubeTransforms[index++] = mat[i];
            }
        }
        //glUniform mat4 light cube map transforms, GL_TRUE is telling GL we are passing in the matrix as row major
        //6 faces and each transform is 16 floats in a 4x4 matrix
        updateUniform("shadowMatrices[0]", lightCubeTransforms);
        delete[] lightCubeTransforms;

        //Set light position for point light
        auto lightPos = light->getPosition().getFlatBuffer();
        updateUniform("lightPos", lightPos);

        //Set far plane for depth scaling
        //Quick trick to get far value out of projection matrix
        auto projMatrix = lightMVP.getProjectionBuffer();
        float nearVal = (2.0f*projMatrix[11]) / (2.0f*projMatrix[10] - 2.0f);
        float farVal = ((projMatrix[10] - 1.0f)*nearVal) / (projMatrix[10] + 1.0f);
        updateUniform("farPlane", &farVal);

        auto textureStrides = model->getTextureStrides();
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
