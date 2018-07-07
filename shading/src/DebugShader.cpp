#include "DebugShader.h"

DebugShader::DebugShader(std::string shaderName) : Shader(shaderName) {

}
void DebugShader::runShader(MVP* mvp, VAO *vao) {
    
    glUseProgram(_shaderContext);
    glBindVertexArray(vao->getVAOContext());

    //glUniform mat4 combined model and world matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    updateUniform("model", mvp->getModelBuffer());

    //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    updateUniform("view", mvp->getViewBuffer());

    //glUniform mat4 projection matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    updateUniform("projection", mvp->getProjectionBuffer());

    //Draw triangles using the bound buffer vertices at starting index 0 and number of triangles
    glDrawArrays(GL_LINES, 0, (GLsizei)vao->getVertexLength());

    glBindVertexArray(0);
    glUseProgram(0);//end using this shader
}
