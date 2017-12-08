#include "DebugShader.h"
#include "Model.h"

DebugShader::DebugShader() {

}

void DebugShader::build() {
    GLhandleARB vertexShaderHandle;
    GLhandleARB fragmentShaderHandle;

    std::string fileNameVert = "shaders/normalShader.vert";
    std::string fileNameFrag = "shaders/normalShader.frag";

    //Compile each shader
    vertexShaderHandle = _compile((char*)fileNameVert.c_str(), GL_VERTEX_SHADER);
    fragmentShaderHandle = _compile((char*)fileNameFrag.c_str(), GL_FRAGMENT_SHADER);

    //Link the two compiled binaries
    _link(vertexShaderHandle, fragmentShaderHandle);
}

void DebugShader::runShader(Model* model) {

    //LOAD IN SHADER
    glUseProgram(_shaderContext); //use context for loaded shader

    //LOAD IN VBO BUFFERS 
    //Bind vertex buff context to current buffer
    glBindBuffer(GL_ARRAY_BUFFER, model->getNormalDebugContext());

    //Say that the vertex data is associated with attribute 0 in the context of a shader program
    //Each vertex contains 3 floats per vertex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //Now enable vertex buffer at location 0
    glEnableVertexAttribArray(0);

    //glUniform mat4 combined model and world matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_modelLocation, 1, GL_TRUE, model->getModelBuffer());

    //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_viewLocation, 1, GL_TRUE, model->getViewBuffer());

    //glUniform mat4 projection matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_projectionLocation, 1, GL_TRUE, model->getProjectionBuffer());

    //Draw normal lines using the bound buffer vertices at starting index 0 and number of lines
    glDrawArraysEXT(GL_LINES, 0, (GLsizei)model->getArrayCount()*2); //Multiply by 2 because each normal is a line

    glDisableVertexAttribArray(0); //Disable vertex attribute
    glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind buffer
    glUseProgram(0);//end using this shader
}