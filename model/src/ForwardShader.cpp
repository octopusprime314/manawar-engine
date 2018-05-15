#include "ForwardShader.h"
#include "MVP.h"
#include "VBO.h"
#include "Model.h"
#include "ViewManager.h"
#include "ShadowRenderer.h"
#include "PointShadowRenderer.h"

ForwardShader::ForwardShader(std::string shaderName) : Shader(shaderName) {
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

    _farPlaneLocation = glGetUniformLocation(_shaderContext, "farPlane");

    //Manually find the two texture locations for loaded shader
   
    _cameraDepthTextureLocation = glGetUniformLocation(_shaderContext, "cameraDepthTexture");
    _mapDepthTextureLocation = glGetUniformLocation(_shaderContext, "mapDepthTexture");
    _lightLocation = glGetUniformLocation(_shaderContext, "light");
    _lightViewLocation = glGetUniformLocation(_shaderContext, "lightViewMatrix");
    _lightMapViewLocation = glGetUniformLocation(_shaderContext, "lightMapViewMatrix");
    _viewsLocation = glGetUniformLocation(_shaderContext, "views");
    _pointLightCountLocation = glGetUniformLocation(_shaderContext, "numPointLights");
    _pointLightColorsLocation = glGetUniformLocation(_shaderContext, "pointLightColors");
    _pointLightRangesLocation = glGetUniformLocation(_shaderContext, "pointLightRanges");
    _pointLightPositionsLocation = glGetUniformLocation(_shaderContext, "pointLightPositions");
    _pointLightDepthMapLocation = glGetUniformLocation(_shaderContext, "depthMap");
    _viewToModelSpaceMatrixLocation = glGetUniformLocation(_shaderContext, "viewToModelMatrix");
}

ForwardShader::~ForwardShader() {

}

void ForwardShader::runShader(Model* model, ViewManager* viewManager, ShadowRenderer* shadowRenderer,
    std::vector<Light*>& lights, PointShadowRenderer* pointShadowRenderer) {
    
    //LOAD IN SHADER
    glUseProgram(_shaderContext); //use context for loaded shader

    //LOAD IN VBO BUFFERS 
    VBO* vbo = model->getVBO();
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

    MVP* mvp = model->getMVP();
    //glUniform mat4 combined model and world matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_modelLocation, 1, GL_TRUE, mvp->getModelBuffer());

    //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_viewLocation, 1, GL_TRUE, mvp->getViewBuffer());

    //glUniform mat4 projection matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_projectionLocation, 1, GL_TRUE, mvp->getProjectionBuffer());

    //glUniform mat4 normal matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_normalLocation, 1, GL_TRUE, mvp->getNormalBuffer());

    auto projMatrix = viewManager->getProjection().getFlatBuffer();
    float nearVal = (2.0f*projMatrix[11]) / (2.0f*projMatrix[10] - 2.0f);
    float farVal = ((projMatrix[10] - 1.0f)*nearVal) / (projMatrix[10] + 1.0f);
    glUniform1f(_farPlaneLocation, farVal);


    //Get light view matrix "look at" vector which is located in the third column
    //of the inner rotation matrix at index 2,6,10
    auto viewMatrix = lights[0]->getMVP().getViewBuffer();
    glUniform3f(_lightLocation, viewMatrix[2], viewMatrix[6], viewMatrix[10]);

    //Get point light positions
    //TODO add max point light constant
    unsigned int pointLights = 0;
    for (auto& light : lights) {
        if (light->getType() == LightType::POINT) {
            pointLights++;
        }
    }
    float* lightPosArray = new float[3 * pointLights];
    float* lightColorsArray = new float[3 * pointLights];
    float* lightRangesArray = new float[pointLights];
    int lightArrayIndex = 0;
    int lightRangeIndex = 0;
    for (auto& light : lights) {
        //If point light then add to uniforms
        if (light->getType() == LightType::POINT) {
            //Point lights need to remain stationary so move lights with camera space changes
            auto pos = viewManager->getView() * light->getPosition();
            float* posBuff = pos.getFlatBuffer();
            float* colorBuff = light->getColor().getFlatBuffer();
            for (int i = 0; i < 3; i++) {
                lightPosArray[lightArrayIndex] = posBuff[i];
                lightColorsArray[lightArrayIndex] = colorBuff[i];
                lightArrayIndex++;
            }
            lightRangesArray[lightRangeIndex++] = light->getRange();
        }
    }
    glUniform1i(_pointLightCountLocation, static_cast<GLsizei>(pointLights));
    glUniform3fv(_pointLightPositionsLocation, static_cast<GLsizei>(pointLights), lightPosArray);
    glUniform3fv(_pointLightColorsLocation, static_cast<GLsizei>(pointLights), lightColorsArray);
    glUniform1fv(_pointLightRangesLocation, static_cast<GLsizei>(pointLights), lightRangesArray);
    delete[] lightPosArray;  delete[] lightColorsArray; delete[] lightRangesArray;

    //Change of basis from camera view position back to world position
    MVP lightMVP = lights[0]->getMVP();
    Matrix cameraToLightSpace = lightMVP.getProjectionMatrix() *
        lightMVP.getViewMatrix() *
        viewManager->getView().inverse();

    //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_lightViewLocation, 1, GL_TRUE, cameraToLightSpace.getFlatBuffer());

    //Change of basis from camera view position back to world position
    MVP lightMapMVP = lights[1]->getMVP();
    Matrix cameraToLightMapSpace = lightMapMVP.getProjectionMatrix() *
        lightMapMVP.getViewMatrix() *
        viewManager->getView().inverse();

    //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_lightMapViewLocation, 1, GL_TRUE, cameraToLightMapSpace.getFlatBuffer());

    //Change of basis from camera view position back to world position
    Matrix viewToModelSpace = viewManager->getView().inverse();

    //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_viewToModelSpaceMatrixLocation, 1, GL_TRUE, viewToModelSpace.getFlatBuffer());

    glUniform1i(_viewsLocation, static_cast<GLint>(viewManager->getViewState()));


    auto textureStrides = model->getTextureStrides();
    unsigned int strideLocation = 0;
    for (auto textureStride : textureStrides) {

        //Do not support layered textures right now
        if (textureStride.first.substr(0, 7) != "Layered") {
            
            //If triangle's textures supports transparency then DO DRAW
            //Only transparent objects are rendered here
            if (model->getTexture(textureStride.first)->getTransparency()) {
               
                //glUniform texture 
                //The second parameter has to be equal to GL_TEXTURE(X) so X must be 0 because we activated texture GL_TEXTURE0 two calls before
                glUniform1i(_textureLocation, 0);
                glUniform1i(_cameraDepthTextureLocation, 1);
                glUniform1i(_mapDepthTextureLocation, 2);
                glUniform1i(_pointLightDepthMapLocation, 3);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, model->getTexture(textureStride.first)->getContext()); //grab first texture of model and return context
                
                //Depth texture
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, shadowRenderer->getAnimatedDepthTexture());

                //Depth texture
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, shadowRenderer->getMapDepthTexture());

                //Depth cube texture map for point lights
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_CUBE_MAP, pointShadowRenderer->getCubeMapDepthTexture());

                //Draw triangles using the bound buffer vertices at starting index 0 and number of triangles
                glDrawArrays(GL_TRIANGLES, strideLocation, (GLsizei)textureStride.second);
            }
        }
        strideLocation += textureStride.second;
    }

    glDisableVertexAttribArray(0); //Disable vertex attribute
    glDisableVertexAttribArray(1); //Disable normal attribute
    glDisableVertexAttribArray(2); //Disable texture attribute
    glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind buffer
    glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture
    glUseProgram(0);//end using this shader

}