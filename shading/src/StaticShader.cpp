#include "StaticShader.h"
#include "Model.h"

StaticShader::StaticShader(std::string shaderName) : Shader(shaderName) {

	//Grab uniforms needed in a staticshader

    //glUniform mat4 combined model and world matrix
    _modelLocation = glGetUniformLocation(_shaderContext, "model");

    //glUniform mat4 view matrix
    _viewLocation = glGetUniformLocation(_shaderContext, "view");

    //glUniform mat4 projection matrix
    _projectionLocation = glGetUniformLocation(_shaderContext, "projection");

    //glUniform mat4 normal matrix
    _normalLocation = glGetUniformLocation(_shaderContext, "normal");

    //glUniform texture map sampler location
    _textureLocation = glGetUniformLocation(_shaderContext, "textureMap");

    //glUniform texture map sampler location
    _tex0Location = glGetUniformLocation(_shaderContext, "tex0");
    _tex1Location = glGetUniformLocation(_shaderContext, "tex1");
    _tex2Location = glGetUniformLocation(_shaderContext, "tex2");
    _tex3Location = glGetUniformLocation(_shaderContext, "tex3");
    _alphatex0Location = glGetUniformLocation(_shaderContext, "alphatex0");

    _layeredSwitchLocation = glGetUniformLocation(_shaderContext, "isLayeredTexture");
}

StaticShader::~StaticShader() {

}

void StaticShader::runShader(Model* model) {

    //LOAD IN SHADER
    glUseProgram(_shaderContext); //use context for loaded shader

    VAO* vao = model->getVAO();
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

    auto textureStrides = model->getTextureStrides();
    unsigned int strideLocation = 0;
    for(auto textureStride : textureStrides) {
        
        //If the texture has layered encoded into the string then it is indeed layered
        if (textureStride.first.substr(0, 7) == "Layered") {

            LayeredTexture* layeredTexture = model->getLayeredTexture(textureStride.first);
            auto textures = layeredTexture->getTextures();

            //We have a layered texture
            glUniform1i(_layeredSwitchLocation, 1);

            if (textures.size() > 4) {

                glUniform1i(_tex0Location, 1);
                glUniform1i(_tex1Location, 2);
                glUniform1i(_tex2Location, 3);
                glUniform1i(_tex3Location, 4);
                glUniform1i(_alphatex0Location, 5);

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, textures[0]->getContext()); //grab first texture of model and return context
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, textures[1]->getContext()); //grab first texture of model and return context
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, textures[2]->getContext()); //grab first texture of model and return context
                glActiveTexture(GL_TEXTURE4);
                glBindTexture(GL_TEXTURE_2D, textures[3]->getContext()); //grab first texture of model and return context
                glActiveTexture(GL_TEXTURE5);
                glBindTexture(GL_TEXTURE_2D, textures[7]->getContext()); //grab first texture of model and return context
            }
            else {

                glUniform1i(_tex0Location, 1);
                glUniform1i(_tex1Location, 2);
                glUniform1i(_tex2Location, 3);
                glUniform1i(_alphatex0Location, 5);

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, textures[0]->getContext()); //grab first texture of model and return context
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, textures[1]->getContext()); //grab first texture of model and return context
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, textures[2]->getContext()); //grab first texture of model and return context
                glActiveTexture(GL_TEXTURE5);
                glBindTexture(GL_TEXTURE_2D, textures[3]->getContext()); //grab first texture of model and return context
            }
            
            
            glDrawArrays(GL_TRIANGLES, strideLocation, (GLsizei)textureStride.second);
            strideLocation += textureStride.second;
        }
        else {
            //If triangle's textures supports transparency then do NOT draw
            //Transparent objects will be rendered after the deferred lighting pass
            if (!model->getTexture(textureStride.first)->getTransparency()) {
                //Not layered texture
                glUniform1i(_layeredSwitchLocation, 0);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, model->getTexture(textureStride.first)->getContext()); //grab first texture of model and return context
                //glUniform texture 
                //The second parameter has to be equal to GL_TEXTURE(X) so X must be 0 because we activated texture GL_TEXTURE0 two calls before
                glUniform1i(_textureLocation, 0);

                //Draw triangles using the bound buffer vertices at starting index 0 and number of triangles
                glDrawArrays(GL_TRIANGLES, strideLocation, (GLsizei)textureStride.second);
            }
            strideLocation += textureStride.second;
        }
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture
    glUseProgram(0);//end using this shader
}

GLint StaticShader::getViewLocation() {
    return _viewLocation;
}

GLint StaticShader::getModelLocation() {
    return _modelLocation;
}

GLint StaticShader::getProjectionLocation() {
    return _projectionLocation;
}

GLint StaticShader::getNormalLocation() {
    return _normalLocation;
}

GLint StaticShader::getTextureLocation() {
    return _textureLocation;
}