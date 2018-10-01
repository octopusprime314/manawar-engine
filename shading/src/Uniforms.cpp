#include "Uniforms.h"
#include <iostream>

Uniforms::Uniforms(GLuint shaderContext) {
    
    int uniformCount = 0;
    glGetProgramiv(shaderContext, GL_ACTIVE_UNIFORMS, &uniformCount);

    for (int i = 0; i < uniformCount; i++) {

        UniformData uniformData = {};
        GLchar      name[64]; 
        glGetActiveUniform(shaderContext, (GLuint)i, sizeof(name), &uniformData.length, &uniformData.size, &uniformData.type, name);
        uniformData.location = glGetUniformLocation(shaderContext, name);
        _uniformMap[name] = uniformData;
    }
}

Uniforms::~Uniforms() {

}

GLint Uniforms::getUniformLocation(std::string uniformName) {

    if (_uniformMap.find(uniformName) == _uniformMap.end()) {
        std::cout << uniformName << " does not exist in this shader!" << std::endl;
        return -1;
    }
    else {
        return _uniformMap[uniformName].location;
    }
}

void Uniforms::updateUniform(std::string uniformName, void* value) {
    
    //Clear out all the errors in the pipe before settings shader uniforms
    while (glGetError() != GL_NO_ERROR);

    //Find the type of the uniform
    switch (_uniformMap[uniformName].type) {
    case GL_FLOAT:
        glUniform1fv(_uniformMap[uniformName].location, _uniformMap[uniformName].size, static_cast<GLfloat*>(value));
        break;
    case GL_FLOAT_VEC2:
        glUniform2fv(_uniformMap[uniformName].location, _uniformMap[uniformName].size, static_cast<GLfloat*>(value));
        break;
    case GL_FLOAT_VEC3:
        glUniform3fv(_uniformMap[uniformName].location, _uniformMap[uniformName].size, static_cast<GLfloat*>(value));
        break;
    case GL_FLOAT_VEC4:
        glUniform4fv(_uniformMap[uniformName].location, _uniformMap[uniformName].size, static_cast<GLfloat*>(value));
        break;
    case GL_INT:
        glUniform1i(_uniformMap[uniformName].location,   * static_cast<GLint*>  (value));
        break;
    case GL_UNSIGNED_INT:
        glUniform1ui(_uniformMap[uniformName].location,  * static_cast<GLuint*> (value));
        break;
    case GL_FLOAT_MAT4:
        glUniformMatrix4fv(_uniformMap[uniformName].location, _uniformMap[uniformName].size, GL_TRUE, static_cast<GLfloat*>(value));
        break;
    default:
        std::cout << "Unknown uniform: " << uniformName << std::endl;
        break;
    }

    if (glGetError() != GL_NO_ERROR) {
        std::cout << "Uniform " << uniformName << " not updated properly in the shader!" << std::endl;
    }
}

void Uniforms::updateUniform(std::string uniformName,
    GLuint textureUnit, 
    GLuint textureContext) {

    //Clear out all the errors in the pipe before settings shader uniforms
    while (glGetError() != GL_NO_ERROR);

    //Find the type of the uniform
    switch (_uniformMap[uniformName].type) {
    case GL_SAMPLER_2D:
        glUniform1i(_uniformMap[uniformName].location, textureUnit - GL_TEXTURE0);
        glActiveTexture(textureUnit);
        glBindTexture(GL_TEXTURE_2D, textureContext);
        break;
    case GL_SAMPLER_CUBE:
        glUniform1i(_uniformMap[uniformName].location, textureUnit - GL_TEXTURE0);
        glActiveTexture(textureUnit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureContext);
        break;
    default:
        std::cout << "Unknown uniform: " << uniformName << std::endl;
        break;
    }

    if (glGetError() != GL_NO_ERROR) {
        std::cout << "Uniform " << uniformName << " not updated properly in the shader!" << std::endl;
    }
}

void Uniforms::updateUniform(std::string uniformName,
    GLuint textureUnit,
    GLuint textureContext,
    ImageData imageInfo) { 

    //Clear out all the errors in the pipe before settings shader uniforms
    while (glGetError() != GL_NO_ERROR);

    //Find the type of the uniform
    switch (_uniformMap[uniformName].type) {
    case GL_IMAGE_2D:
        glUniform1i(_uniformMap[uniformName].location, textureUnit - GL_TEXTURE0);
        if (imageInfo.readOnly) {
            glBindImageTexture(textureUnit - GL_TEXTURE0, textureContext, 0, GL_FALSE, 0, GL_READ_ONLY, imageInfo.format);
        }
        else {
            glBindImageTexture(textureUnit - GL_TEXTURE0, textureContext, 0, GL_FALSE, 0, GL_WRITE_ONLY, imageInfo.format);
        }
        break;
    default:
        std::cout << "Unknown uniform: " << uniformName << std::endl;
        break;
    }

    if (glGetError() != GL_NO_ERROR) {
        std::cout << "Uniform " << uniformName << " not updated properly in the shader!" << std::endl;
    }
}