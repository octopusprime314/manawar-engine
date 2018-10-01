#include "AnimationShader.h"
#include "AnimatedModel.h"
#include "Entity.h"

AnimationShader::AnimationShader(std::string shaderName) : StaticShader(shaderName) {

}

AnimationShader::~AnimationShader() {

}

void AnimationShader::runShader(Entity* entity) {

    AnimatedModel* model = static_cast<AnimatedModel*>(entity->getModel());

    std::vector<VAO*>* vao = entity->getFrustumVAO();
    //LOAD IN SHADER
    _shader->bind();
    unsigned int id = entity->getID();
    _shader->updateData("id", &id);

    for (auto vaoInstance : *vao) {
        glBindVertexArray(vaoInstance->getVAOContext());

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

        //Bone uniforms
        auto bones = model->getBones();
        float* bonesArray = new float[16 * 150]; //4x4 times number of bones
        int bonesArrayIndex = 0;
        for (auto bone : *bones) {
            for (int i = 0; i < 16; i++) {
                float* buff = bone.getFlatBuffer();
                bonesArray[bonesArrayIndex++] = buff[i];
            }
        }
        _shader->updateData("bones[0]", bonesArray);
        delete[] bonesArray;

        //Grab strides of vertex sets that have a single texture associated with them
        auto textureStrides = vaoInstance->getTextureStrides();
        unsigned int strideLocation = 0;
        for (auto textureStride : textureStrides) {

            _shader->updateData("textureMap", GL_TEXTURE0, model->getTexture(textureStride.first));

            //Draw triangles using the bound buffer vertices at starting index 0 and number of vertices
            glDrawArrays(GL_TRIANGLES, strideLocation, (GLsizei)textureStride.second);

            strideLocation += textureStride.second;
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture
    }
    glUseProgram(0);//end using this shader
}
