#include "ShadowStaticShader.h"
#include "Entity.h"
#include "GLSLShader.h"
#include "HLSLShader.h"
#include "EngineManager.h"

ShadowStaticShader::ShadowStaticShader(std::string shaderName) {

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        _shader = new GLSLShader(shaderName);
    }
    else {
        std::vector<DXGI_FORMAT>* formats = new std::vector<DXGI_FORMAT>();
        formats->push_back(DXGI_FORMAT_D32_FLOAT);
        _shader = new HLSLShader(shaderName, "", formats);
    }
}

ShadowStaticShader::~ShadowStaticShader() {

}

void ShadowStaticShader::runShader(Entity* entity, Light* light) {

    //Load in vbo buffers
    auto model = entity->getModel();
    std::vector<VAO*>* vao = model->getVAO();
    MVP* modelMVP = entity->getMVP();
    MVP lightMVP = light->getLightMVP();

    //LOAD IN SHADER
    _shader->bind();

    for (auto vaoInstance : *vao) {

        _shader->bindAttributes(vaoInstance);

        MVP* mvp = entity->getMVP();
        //glUniform mat4 combined model and world matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        _shader->updateData("model", mvp->getModelBuffer());

        //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        _shader->updateData("view", lightMVP.getViewBuffer());

        //glUniform mat4 projection matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        _shader->updateData("projection", lightMVP.getProjectionBuffer());

        auto textureStrides = vaoInstance->getTextureStrides();
        unsigned int verticesSize = 0;
        for (auto textureStride : textureStrides) {
            verticesSize += textureStride.second;
        }

        //Draw triangles using the bound buffer vertices at starting index 0 and number of vertices
        _shader->draw(0, 1, (GLsizei)verticesSize);

        _shader->unbindAttributes();
    }
    _shader->unbind();
}
