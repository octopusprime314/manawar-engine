#include "StaticShader.h"
#include "Model.h"
#include "Entity.h"
#include "ModelBroker.h"
#include "GLSLShader.h"
#include "HLSLShader.h"
#include "EngineManager.h"
#include "DXLayer.h"

StaticShader::StaticShader(std::string shaderName) {

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        _shader = new GLSLShader(shaderName);
    }
    else {
        _shader = new HLSLShader(shaderName);
    }
}

StaticShader::~StaticShader() {

}

void StaticShader::runShader(Entity* entity) {

    //LOAD IN SHADER
    _shader->bind();

    auto model = entity->getModel();
    unsigned int id = entity->getID();
    _shader->updateData("id", &id);

    std::vector<VAO*>* vao = entity->getFrustumVAO(); //Special vao call that factors in frustum culling for the scene
    for (auto vaoInstance : *vao) {
        _shader->bindAttributes(vaoInstance);
        
        unsigned int primitiveOffsetId = vaoInstance->getPrimitiveOffsetId();
       
        MVP* mvp = entity->getMVP();

        //glUniform mat4 combined model and world matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        _shader->updateData("model", mvp->getModelBuffer());

        //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        _shader->updateData("view", mvp->getViewBuffer());

        //glUniform mat4 projection matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        _shader->updateData("projection", mvp->getProjectionBuffer());

        //glUniform mat4 normal matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        _shader->updateData("normal", mvp->getNormalBuffer());

        MVP* prevMVP = entity->getPrevMVP();
        //glUniform mat4 combined model and world matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        _shader->updateData("prevModel", prevMVP->getModelBuffer());

        //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        _shader->updateData("prevView", prevMVP->getViewBuffer());

        auto textureStrides = vaoInstance->getTextureStrides();
        unsigned int strideLocation = 0;
        for (auto textureStride : textureStrides) {

            _shader->updateData("primitiveOffset", &primitiveOffsetId);
            //If the texture has layered encoded into the string then it is indeed layered
            if (textureStride.first.substr(0, 7) == "Layered") {

                LayeredTexture* layeredTexture = entity->getLayeredTexture();

                auto textures = layeredTexture->getTextures();

                //We have a layered texture
                int isLayered = 1;
                _shader->updateData("isLayeredTexture", &isLayered);

                _shader->updateData("tex0", GL_TEXTURE1, textures[0]);
                _shader->updateData("tex1", GL_TEXTURE2, textures[1]);
                _shader->updateData("tex2", GL_TEXTURE3, textures[2]);
                _shader->updateData("tex3", GL_TEXTURE4, textures[3]);
                _shader->updateData("alphatex0", GL_TEXTURE5, textures[4]);
                
                _shader->draw(strideLocation, 1, (GLsizei)textureStride.second);
                strideLocation += textureStride.second;
                primitiveOffsetId += (textureStride.second / 3);
            }
            else {
                //If triangle's textures supports transparency then do NOT draw
                //Transparent objects will be rendered after the deferred lighting pass
                if (!model->getTexture(textureStride.first)->getTransparency()) {

                    //Not a layered texture
                    int isLayered = 0;
                    _shader->updateData("isLayeredTexture", &isLayered);

                    _shader->updateData("textureMap", GL_TEXTURE0, model->getTexture(textureStride.first));

                    //Draw triangles using the bound buffer vertices at starting index 0 and number of triangles
                    _shader->draw(strideLocation, 1, (GLsizei)textureStride.second);
                }
                strideLocation += textureStride.second;
            }
        }
        _shader->unbindAttributes();
    }
    _shader->unbind();
}
