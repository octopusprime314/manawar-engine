#include "EffectShader.h"
#include "Light.h"
#include "ViewManager.h"

EffectShader::EffectShader(std::string shaderName) : Shader(shaderName) {
    _timeLocation = glGetUniformLocation(_shaderContext, "time");
    _modelViewProjectionLocation = glGetUniformLocation(_shaderContext, "mvp");
    _lightPosLocation = glGetUniformLocation(_shaderContext, "lightPos");
    _fireTypeLocation = glGetUniformLocation(_shaderContext, "fireType");
    _farPlaneLocation = glGetUniformLocation(_shaderContext, "farPlane");
    glGenVertexArrays(1, &_vaoContext);
}

EffectShader::~EffectShader() {

}

void EffectShader::runShader(Light* light, MVP& cameraMVP, float seconds) {

    //LOAD IN SHADER
    glUseProgram(_shaderContext); 

    glBindVertexArray(_vaoContext);

    //Pass game time to shader
    glUniform1f(_timeLocation, seconds);

    //Pass far plane to shader
    auto projMatrix = cameraMVP.getProjectionMatrix().getFlatBuffer();
    float nearVal = (2.0f*projMatrix[11]) / (2.0f*projMatrix[10] - 2.0f);
    float farVal = ((projMatrix[10] - 1.0f)*nearVal) / (projMatrix[10] + 1.0f);
    glUniform1f(_farPlaneLocation, farVal);

    //Pass the type of fire to the shader to simulate i.e. candle light or bon fire
    glUniform1i(_fireTypeLocation, 2);

    auto MVP = light->getMVP();

    //Transform screen space quad to the MVP of the emitting light
    auto cameraMatPtr = cameraMVP.getViewBuffer();
    //Now we also want to billboard this screen space effect so make the inner rotation matrix an identity
    //and just take the translation from the camera view matrix
    Matrix cameraNoRotationMat = Matrix::translation(cameraMatPtr[3], cameraMatPtr[7], cameraMatPtr[11]);
    auto modelViewProjection = cameraMVP.getProjectionMatrix() * cameraNoRotationMat * MVP.getModelMatrix();
    glUniformMatrix4fv(_modelViewProjectionLocation, 1, GL_TRUE, modelViewProjection.getFlatBuffer());

    //screen space quad
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)4);
    
    glBindVertexArray(0);

    glUseProgram(0);//end using this shader
}