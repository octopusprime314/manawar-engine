#include "ShadowAnimatedShader.h"

ShadowAnimatedShader::ShadowAnimatedShader(std::string shaderName) : ShadowStaticShader(shaderName) {

	//Grab uniforms needed in a staticshader

    //glUniform mat4 combined model and world matrix
    _bonesLocation = glGetUniformLocation(_shaderContext, "bones");
}

ShadowAnimatedShader::~ShadowAnimatedShader(){

}

void ShadowAnimatedShader::runShader(Model* model, Light* light) {

	AnimatedModel* animationModel = static_cast<AnimatedModel*>(model);

	//Load in vbo buffers
    VBO* vbo = animationModel->getVBO();
    MVP* modelMVP = animationModel->getMVP();
	MVP lightMVP = light->getMVP();

    //Use one single shadow shader and replace the vbo buffer from each model
    glUseProgram(_shaderContext); //use context for loaded shader

    //Bind vertex buff context to current buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo->getVertexContext());

    //Say that the vertex data is associated with attribute 0 in the context of a shader program
    //Each vertex contains 3 floats per vertex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //Now enable vertex buffer at location 0
    glEnableVertexAttribArray(0);

    //Bind bone index buff context to current buffer
    glBindBuffer(GL_ARRAY_BUFFER, animationModel->getIndexContext());

    //First 4 indexes
    //Specify stride to be 8 because the beginning of each index attribute value is 8 bytes away
    //Say that the bone index data is associated with attribute 3 in the context of a shader program
    //Each bone index contains 4 floats per index
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), 0);

    //Now enable bone index buffer at location 1
    glEnableVertexAttribArray(1);

    //Second 4 indexes
    //Specify stride to be 8 because the beginning of each index attribute value is 8 bytes away
    //Specify offset for attribute location of indexes2 to be 4 bytes offset from the beginning location of the buffer
    //Say that the bone index data is associated with attribute 3 in the context of a shader program
    //Each bone index contains 4 floats per index
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(4 * sizeof(GL_FLOAT)));

    //Now enable bone index buffer at location 2
    glEnableVertexAttribArray(2);

    //Bind weight buff context to current buffer
    glBindBuffer(GL_ARRAY_BUFFER, animationModel->getWeightContext());

    //First 4 weights
    //Specify stride to be 8 because the beginning of each weight attribute value is 8 bytes away
    //Say that the weight data is associated with attribute 4 in the context of a shader program
    //Each weight contains 4 floats per index
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), 0);

    //Now enable weight buffer at location 3
    glEnableVertexAttribArray(3);

    //Second 4 weights
    //Specify stride to be 8 because the beginning of each weight attribute value is 8 bytes away
    //Specify offset for attribute location of weights2 to be 4 bytes offset from the beginning location of the buffer
    //Say that the weight data is associated with attribute 4 in the context of a shader program
    //Each weight contains 4 floats per index
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(4 * sizeof(GL_FLOAT)));

    //Now enable weight buffer at location 4
    glEnableVertexAttribArray(4);

    //glUniform mat4 combined model and world matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_modelLocation, 1, GL_TRUE, modelMVP->getModelBuffer());

    //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_viewLocation, 1, GL_TRUE, lightMVP.getViewBuffer());

    //glUniform mat4 projection matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_projectionLocation, 1, GL_TRUE, lightMVP.getProjectionBuffer());

    //Bone uniforms
    auto bones = animationModel->getBones();
    float* bonesArray = new float[16 * bones->size()]; //4x4 times number of bones
    int bonesArrayIndex = 0;
    for (auto bone : *bones) {
        for (int i = 0; i < 16; i++) {
            float* buff = bone.getFlatBuffer();
            bonesArray[bonesArrayIndex++] = buff[i];
        }
    }
    glUniformMatrix4fv(_bonesLocation, static_cast<GLsizei>(bones->size()), GL_TRUE, bonesArray);
    delete[] bonesArray;

    auto textureStrides = model->getTextureStrides();
    unsigned int verticesSize = 0;
    for (auto textureStride : textureStrides) {
        verticesSize += textureStride.second;
    }

    //Draw triangles using the bound buffer vertices at starting index 0 and number of vertices
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)verticesSize);

    glDisableVertexAttribArray(0); //Disable vertex attribute
    glDisableVertexAttribArray(1); //Disable indexes attribute
    glDisableVertexAttribArray(2); //Disable weight attribute
    glDisableVertexAttribArray(3); //Disable indexes2 attribute
    glDisableVertexAttribArray(4); //Disable weight2 attribute
    glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind buffer
    glUseProgram(0);//end using this shader
}
