#include "AnimationShader.h"
#include "AnimatedModel.h"

AnimationShader::AnimationShader(std::string shaderName) : StaticShader(shaderName) {
	//Grab uniforms needed in a staticshader

    //glUniform mat4 combined model and world matrix
    _bonesLocation = glGetUniformLocation(_shaderContext, "bones");
}

AnimationShader::~AnimationShader() {

}

void AnimationShader::runShader(Model* modelIn) {

    AnimatedModel* model = static_cast<AnimatedModel*>(modelIn);

    //LOAD IN SHADER
    VAO* vao = model->getVAO();
    glUseProgram(_shaderContext); //use context for loaded shader

    glBindVertexArray(vao->getVAOContext());

    MVP* mvp = model->getMVP();
    //glUniform mat4 combined model and world matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_modelLocation, 1, GL_TRUE, mvp->getModelBuffer());

    //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_viewLocation, 1, GL_TRUE, mvp->getViewBuffer());

    //glUniform mat4 projection matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_projectionLocation, 1, GL_TRUE, mvp->getProjectionBuffer());

    //glUniform mat4 normal matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_normalLocation, 1, GL_TRUE, mvp->getNormalBuffer());

    //Bone uniforms
    auto bones = model->getBones();
    float* bonesArray = new float[ 16 * bones->size() ]; //4x4 times number of bones
    int bonesArrayIndex = 0;
    for(auto bone : *bones) {
        for(int i = 0; i < 16; i++) {
            float* buff = bone.getFlatBuffer();
            bonesArray[bonesArrayIndex++] = buff[i];
        }
    }
    glUniformMatrix4fv(_bonesLocation, static_cast<GLsizei>(bones->size()), GL_TRUE, bonesArray);
    delete [] bonesArray;

    //Grab strides of vertex sets that have a single texture associated with them
    auto textureStrides = model->getTextureStrides();
    unsigned int strideLocation = 0;
    for(auto textureStride : textureStrides) {

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, model->getTexture(textureStride.first)->getContext()); //grab first texture of model and return context
        //glUniform texture
        //The second parameter has to be equal to GL_TEXTURE(X) so X must be 0 because we activated texture GL_TEXTURE0 two calls before
        glUniform1i(_textureLocation, 0);

        //Draw triangles using the bound buffer vertices at starting index 0 and number of vertices
        glDrawArrays(GL_TRIANGLES, strideLocation, (GLsizei)textureStride.second);

        strideLocation += textureStride.second;

    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture
    glUseProgram(0);//end using this shader
}
