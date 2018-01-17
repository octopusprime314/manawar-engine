#include "AnimationShader.h"
#include "AnimatedModel.h"

AnimationShader::AnimationShader() {

}

AnimationShader::~AnimationShader() {

}

void AnimationShader::build(std::string shaderName) {

    //Call main build
    Shader::build("shaders/animationShader"/*shaderName*/);

    //Boen uniforms glUniform mat4 
    _bonesLocation = glGetUniformLocation(_shaderContext, "bones");

}

void AnimationShader::runShader(Model* modelIn) {

    AnimatedModel* model = static_cast<AnimatedModel*>(modelIn);

    //LOAD IN SHADER
    VBO* vbo = model->getVBO();
    glUseProgram(_shaderContext); //use context for loaded shader

    //LOAD IN VBO BUFFERS 
    //Bind vertex buff context to current buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo->getVertexContext());

    //Say that the vertex data is associated with attribute 0 in the context of a shader program
    //Each vertex contains 3 floats per vertex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //Now enable vertex buffer at location 0
    glEnableVertexAttribArray(0);

    //Bind normal buff context to current buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo->getNormalContext());

    //Say that the normal data is associated with attribute 1 in the context of a shader program
    //Each normal contains 3 floats per normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //Now enable normal buffer at location 1
    glEnableVertexAttribArray(1);

    //Bind texture coordinate buff context to current buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo->getTextureContext());

    //Say that the texture coordinate data is associated with attribute 2 in the context of a shader program
    //Each texture coordinate contains 2 floats per texture
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

    //Now enable texture buffer at location 2
    glEnableVertexAttribArray(2);

    //Bind bone index buff context to current buffer
    glBindBuffer(GL_ARRAY_BUFFER, model->getIndexContext());

    //First 4 indexes
    //Specify stride to be 8 because the beginning of each index attribute value is 8 bytes away
    //Say that the bone index data is associated with attribute 3 in the context of a shader program
    //Each bone index contains 4 floats per index
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), 0);

    //Now enable bone index buffer at location 3
    glEnableVertexAttribArray(3);

    //Second 4 indexes
    //Specify stride to be 8 because the beginning of each index attribute value is 8 bytes away
    //Specify offset for attribute location of indexes2 to be 4 bytes offset from the beginning location of the buffer
    //Say that the bone index data is associated with attribute 3 in the context of a shader program
    //Each bone index contains 4 floats per index
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(4 * sizeof(GL_FLOAT)));

    //Now enable bone index buffer at location 3
    glEnableVertexAttribArray(4);

    //Bind weight buff context to current buffer
    glBindBuffer(GL_ARRAY_BUFFER, model->getWeightContext());

    //First 4 weights
    //Specify stride to be 8 because the beginning of each weight attribute value is 8 bytes away
    //Say that the weight data is associated with attribute 4 in the context of a shader program
    //Each weight contains 4 floats per index
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), 0);

    //Now enable weight buffer at location 4
    glEnableVertexAttribArray(5);

    //Second 4 weights
    //Specify stride to be 8 because the beginning of each weight attribute value is 8 bytes away
    //Specify offset for attribute location of weights2 to be 4 bytes offset from the beginning location of the buffer
    //Say that the weight data is associated with attribute 4 in the context of a shader program
    //Each weight contains 4 floats per index
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(4 * sizeof(GL_FLOAT)));

    //Now enable weight buffer at location 4
    glEnableVertexAttribArray(6);

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
        glUniform1iARB(_textureLocation, 0); 

        //Draw triangles using the bound buffer vertices at starting index 0 and number of vertices
        glDrawArrays(GL_TRIANGLES, strideLocation, (GLsizei)textureStride.second);

        strideLocation += textureStride.second;

    }

    glDisableVertexAttribArray(0); //Disable vertex attribute
    glDisableVertexAttribArray(1); //Disable normal attribute
    glDisableVertexAttribArray(2); //Disable texture attribute
    glDisableVertexAttribArray(3); //Disable indexes attribute
    glDisableVertexAttribArray(4); //Disable weight attribute
    glDisableVertexAttribArray(5); //Disable indexes2 attribute
    glDisableVertexAttribArray(6); //Disable weight2 attribute
    glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind buffer
    glUseProgram(0);//end using this shader
}