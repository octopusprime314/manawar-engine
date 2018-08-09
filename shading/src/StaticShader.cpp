#include "StaticShader.h"
#include "Model.h"
#include "Entity.h"
#include "ModelBroker.h"

StaticShader::StaticShader(std::string shaderName) : Shader(shaderName) {

}

StaticShader::~StaticShader() {

}

void StaticShader::runShader(Entity* entity) {

    //LOAD IN SHADER
    glUseProgram(_shaderContext); //use context for loaded shader
    auto model = entity->getModel();
    auto baseID = entity->getID();
    std::vector<VAO*>* vao = entity->getFrustumVAO(); //Special vao call that factors in frustum culling for the scene
    //std::map<unsigned int, unsigned int> primitiveOffsetMap;
    for (auto vaoInstance : *vao) {
        glBindVertexArray(vaoInstance->getVAOContext());
        
        unsigned int id = entity->getID();
        unsigned int primitiveOffsetId = vaoInstance->setPrimitiveOffsetId();
       
        MVP* mvp = entity->getMVP();
        //glUniform mat4 combined model and world matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        updateUniform("model", mvp->getModelBuffer());

        //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        updateUniform("view", mvp->getViewBuffer());

        //glUniform mat4 projection matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        updateUniform("projection", mvp->getProjectionBuffer());

        //glUniform mat4 normal matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        updateUniform("normal", mvp->getNormalBuffer());

        MVP* prevMVP = entity->getPrevMVP();
        //glUniform mat4 combined model and world matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        updateUniform("prevModel", prevMVP->getModelBuffer());

        //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        updateUniform("prevView", prevMVP->getViewBuffer());

        
        auto textureStrides = vaoInstance->getTextureStrides();
        unsigned int strideLocation = 0;
        for (auto textureStride : textureStrides) {

            updateUniform("primitiveOffset", &primitiveOffsetId);
            //If the texture has layered encoded into the string then it is indeed layered
            if (textureStride.first.substr(0, 7) == "Layered") {

                LayeredTexture* layeredTexture = model->getLayeredTexture(textureStride.first);
                auto textures = layeredTexture->getTextures();

                //We have a layered texture
                int isLayered = 1;
                updateUniform("isLayeredTexture", &isLayered);

                updateUniform("tex0", GL_TEXTURE1, textures[0]->getContext());
                updateUniform("tex1", GL_TEXTURE2, textures[1]->getContext());
                updateUniform("tex2", GL_TEXTURE3, textures[2]->getContext());
                updateUniform("tex3", GL_TEXTURE4, textures[3]->getContext());
                updateUniform("alphatex0", GL_TEXTURE5, textures[4]->getContext());
                
                glDrawArrays(GL_TRIANGLES, strideLocation, (GLsizei)textureStride.second);
                strideLocation += textureStride.second;
                primitiveOffsetId += (textureStride.second / 3);
            }
            else {
                //If triangle's textures supports transparency then do NOT draw
                //Transparent objects will be rendered after the deferred lighting pass
                if (!model->getTexture(textureStride.first)->getTransparency()) {

                    //Not a layered texture
                    int isLayered = 0;
                    updateUniform("isLayeredTexture", &isLayered);

                    updateUniform("textureMap", GL_TEXTURE0, model->getTexture(textureStride.first)->getContext());

                    //Draw triangles using the bound buffer vertices at starting index 0 and number of triangles
                    glDrawArrays(GL_TRIANGLES, strideLocation, (GLsizei)textureStride.second);
                }
                strideLocation += textureStride.second;
            }
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture
    }
    glUseProgram(0);//end using this shader
}
