#include "ShadowPointShader.h"

ShadowPointShader::ShadowPointShader(std::string shaderName) : Shader(shaderName){

    _modelLocation = glGetUniformLocation(_shaderContext, "model");
    _viewLocation = glGetUniformLocation(_shaderContext, "view");
    _lightCubeTransformsLocation = glGetUniformLocation(_shaderContext, "shadowMatrices");
    _lightPosLocation = glGetUniformLocation(_shaderContext, "lightPos");
    _farPlaneLocation = glGetUniformLocation(_shaderContext, "farPlane");
}

ShadowPointShader::~ShadowPointShader() {

}

void ShadowPointShader::runShader(Model* model, Light* light, std::vector<Matrix> lightTransforms) {

    //Load in vbo buffers
    VBO* vbo = model->getVBO();
    MVP* modelMVP = model->getMVP();
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

    //glUniform mat4 combined model and world matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_modelLocation, 1, GL_TRUE, modelMVP->getModelBuffer());

    //glUniform mat4 combined model and world matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_viewLocation, 1, GL_TRUE, modelMVP->getViewBuffer());

    float* lightCubeTransforms = new float[6*16];
    int index = 0;
    for (Matrix lightTransform : lightTransforms) {
        float* mat = lightTransform.getFlatBuffer();
        for (int i = 0; i < 16; ++i) {
            lightCubeTransforms[index++] = mat[i];
        }
    }
    //glUniform mat4 light cube map transforms, GL_TRUE is telling GL we are passing in the matrix as row major
    //6 faces and each transform is 16 floats in a 4x4 matrix
    glUniformMatrix4fv(_lightCubeTransformsLocation, 6, GL_TRUE, lightCubeTransforms);
    delete[] lightCubeTransforms;

    //Set light position for point light
    auto lightPos = light->getPosition();
    glUniform3f(_lightPosLocation, lightPos.getx(), lightPos.gety(), lightPos.getz());
    
    //Set far plane for depth scaling
    //Quick trick to get far value out of projection matrix
    auto projMatrix = lightMVP.getProjectionBuffer();
    float nearVal = (2.0f*projMatrix[11]) / (2.0f*projMatrix[10] - 2.0f);
    float farVal = ((projMatrix[10] - 1.0f)*nearVal) / (projMatrix[10] + 1.0f);
    glUniform1f(_farPlaneLocation, farVal);

    auto textureStrides = model->getTextureStrides();
    unsigned int verticesSize = 0;
    for (auto textureStride : textureStrides) {
        verticesSize += textureStride.second;
    }

    //Draw triangles using the bound buffer vertices at starting index 0 and number of vertices
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)verticesSize);

    glDisableVertexAttribArray(0); //Disable vertex attribute
    glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind buffer
    glUseProgram(0);//end using this shader
}