#include "ShadowStaticShader.h"
#include "Entity.h"

ShadowStaticShader::ShadowStaticShader(std::string shaderName) : Shader(shaderName) {

}

ShadowStaticShader::~ShadowStaticShader() {

}

void ShadowStaticShader::runShader(Entity* entity, Light* light) {

    //Load in vbo buffers
    auto model = entity->getModel();
    std::vector<VAO*>* vao = model->getVAO();
    MVP* modelMVP = entity->getMVP();
    MVP lightMVP = light->getLightMVP();

    //Use one single shadow shader and replace the vbo buffer from each model
    glUseProgram(_shaderContext); //use context for loaded shader

    for (auto vaoInstance : *vao) {
        glBindVertexArray(vaoInstance->getVAOShadowContext());

        MVP* mvp = entity->getMVP();
        //glUniform mat4 combined model and world matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        updateUniform("model", mvp->getModelBuffer());

        //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        updateUniform("view", lightMVP.getViewBuffer());

        //glUniform mat4 projection matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        updateUniform("projection", lightMVP.getProjectionBuffer());

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
