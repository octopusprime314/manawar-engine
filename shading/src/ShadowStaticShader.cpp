#include "ShadowStaticShader.h"

ShadowStaticShader::ShadowStaticShader(std::string shaderName) : ShadowShader(shaderName) {

	//Grab uniforms needed in a staticshader

    //glUniform mat4 combined model and world matrix
    _modelLocation = glGetUniformLocation(_shaderContext, "model");

    //glUniform mat4 view matrix
    _viewLocation = glGetUniformLocation(_shaderContext, "view");

    //glUniform mat4 projection matrix
    _projectionLocation = glGetUniformLocation(_shaderContext, "projection");
}

ShadowStaticShader::~ShadowStaticShader(){

}

void ShadowStaticShader::runShader(Model* model, Light* light) {

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

    //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_viewLocation, 1, GL_TRUE, lightMVP.getViewBuffer());

    //glUniform mat4 projection matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_projectionLocation, 1, GL_TRUE, lightMVP.getProjectionBuffer());

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

GLint ShadowStaticShader::getViewLocation() {
    return _viewLocation;
}

GLint ShadowStaticShader::getModelLocation() {
    return _modelLocation;
}

GLint ShadowStaticShader::getProjectionLocation() {
    return _projectionLocation;
}