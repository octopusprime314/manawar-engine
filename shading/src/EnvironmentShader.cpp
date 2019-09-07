#include "EnvironmentShader.h"
#include "Model.h"
#include "Entity.h"
#include "GLSLShader.h"
#include "HLSLShader.h"
#include "EngineManager.h"

EnvironmentShader::EnvironmentShader(std::string shaderName){
    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        _shader = new GLSLShader(shaderName);
    }
    else {
        _shader = new HLSLShader(shaderName);
    }
}

EnvironmentShader::~EnvironmentShader() {

}

void EnvironmentShader::runShader(Entity*             entity,
                                  std::vector<Matrix> viewTransforms) {

    //LOAD IN SHADER
    _shader->bind();

    auto model             = entity->getModel();
    std::vector<VAO*>* vao = model->getVAO();
    for (auto vaoInstance : *vao) {
        glBindVertexArray(vaoInstance->getVAOContext());

        MVP* mvp = entity->getMVP();
        _shader->updateData("model", mvp->getModelBuffer());
        _shader->updateData("view",  mvp->getViewBuffer());

        float* lightCubeTransforms = new float[6 * 16];
        int index                  = 0;
        for (Matrix lightTransform : viewTransforms) {
            float* mat = lightTransform.getFlatBuffer();
            for (int i = 0; i < 16; ++i) {
                lightCubeTransforms[index++] = mat[i];
            }
        }
        //glUniform mat4 light cube map transforms, GL_TRUE is telling GL we are passing in the matrix as row major
        //6 faces and each transform is 16 floats in a 4x4 matrix
        _shader->updateData("viewMatrices[0]", lightCubeTransforms);
        delete[] lightCubeTransforms;

        auto textureStrides         = vaoInstance->getTextureStrides();
        unsigned int strideLocation = 0;
        for (auto textureStride : textureStrides) {

            //If the texture has layered encoded into the string then it is indeed layered
            if (textureStride.first.substr(0, 7) == "Layered") {

                LayeredTexture* layeredTexture = model->getLayeredTexture(textureStride.first);
                auto textures                  = layeredTexture->getTextures();

                //We have a layered texture
                int isLayered = 1;
                _shader->updateData("isLayeredTexture", &isLayered);

                if (textures.size() > 4) {
                    _shader->updateData("tex0",      GL_TEXTURE1, textures[0]);
                    _shader->updateData("tex1",      GL_TEXTURE2, textures[1]);
                    _shader->updateData("tex2",      GL_TEXTURE3, textures[2]);
                    _shader->updateData("tex3",      GL_TEXTURE4, textures[3]);
                    _shader->updateData("alphatex0", GL_TEXTURE5, textures[7]);
                }
                else {
                    _shader->updateData("tex0",      GL_TEXTURE1, textures[0]);
                    _shader->updateData("tex1",      GL_TEXTURE2, textures[1]);
                    _shader->updateData("tex2",      GL_TEXTURE3, textures[2]);
                    _shader->updateData("alphatex0", GL_TEXTURE5, textures[3]);
                }
                glDrawArrays(GL_TRIANGLES, strideLocation, (GLsizei)textureStride.second);
                strideLocation += textureStride.second;
            }
            else {
                //If triangle's textures supports transparency then do NOT draw
                //Transparent objects will be rendered after the deferred lighting pass
                if (!model->getTexture(textureStride.first)->getTransparency()) {
                    //Not layered texture
                    int isLayered = 0;
                    _shader->updateData("isLayeredTexture", &isLayered);
                    _shader->updateData("textureMap", GL_TEXTURE0, model->getTexture(textureStride.first));

                    //Draw triangles using the bound buffer vertices at starting index 0 and number of triangles
                    glDrawArrays(GL_TRIANGLES, strideLocation, (GLsizei)textureStride.second);
                }
                strideLocation += textureStride.second;
            }
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glUseProgram(0);
}
