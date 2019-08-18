#include "ShadowAnimatedShader.h"
#include "Entity.h"

ShadowAnimatedShader::ShadowAnimatedShader(std::string shaderName) : ShadowStaticShader(shaderName) {

}

ShadowAnimatedShader::~ShadowAnimatedShader() {

}

void ShadowAnimatedShader::runShader(Entity* entity, Light* light) {

    AnimatedModel* animationModel = static_cast<AnimatedModel*>(entity->getModel());
    //Load in vbo buffers
    std::vector<VAO*>* vao        = animationModel->getVAO();
    MVP* modelMVP                 = entity->getMVP();
    MVP lightMVP                  = light->getLightMVP();

    //Use one single shadow shader and replace the vbo buffer from each model
    _shader->bind();

    for (auto vaoInstance : *vao) {
        _shader->bindAttributes(vaoInstance);

        _shader->updateData("model",      modelMVP->getModelBuffer());
        _shader->updateData("view",       lightMVP.getViewBuffer());
        _shader->updateData("projection", lightMVP.getProjectionBuffer());

        //Bone uniforms
        auto bones          = animationModel->getBones();
        float* bonesArray   = new float[16 * 150]; //4x4 times number of bones
        int bonesArrayIndex = 0;
        for (auto bone : *bones) {
            for (int i = 0; i < 16; i++) {
                float* buff = bone.getFlatBuffer();
                bonesArray[bonesArrayIndex++] = buff[i];
            }
        }
        _shader->updateData("bones[0]", bonesArray);
        delete[] bonesArray;

        auto textureStrides       = vaoInstance->getTextureStrides();
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
