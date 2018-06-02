#include "EffectShader.h"
#include "Light.h"
#include "ViewManager.h"

EffectShader::EffectShader(std::string shaderName) : Shader(shaderName) {

    //Build 2 triangles for screen space quad
    const float length = 1.0f;
    const float depth = 0.0f;
    //2 triangles in screen space 
    float triangles[] = { -length, -length, depth,
        -length, length, depth,
        length, length, depth,

        -length, -length, depth,
        length, length, depth,
        length, -length, depth };

    glGenBuffers(1, &_quadBufferContext);
    glBindBuffer(GL_ARRAY_BUFFER, _quadBufferContext);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 3, triangles, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &_vaoContext);
    glBindVertexArray(_vaoContext);

    glBindBuffer(GL_ARRAY_BUFFER, _quadBufferContext);

    //Say that the vertex data is associated with attribute 0 in the context of a shader program
    //Each vertex contains 3 floats per vertex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //Now enable vertex buffer at location 0
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    _timeLocation = glGetUniformLocation(_shaderContext, "time");
    _modelViewLocation = glGetUniformLocation(_shaderContext, "mv");
    _projectionLocation = glGetUniformLocation(_shaderContext, "p");
    _inverseViewLocation = glGetUniformLocation(_shaderContext, "inverseViewNoTrans");
    _lightPosLocation = glGetUniformLocation(_shaderContext, "lightPos");
    _fireTypeLocation = glGetUniformLocation(_shaderContext, "fireType");
    _farPlaneLocation = glGetUniformLocation(_shaderContext, "farPlane");
}

EffectShader::~EffectShader() {

}

void EffectShader::runShader(Light* light, float seconds) {

    //LOAD IN SHADER
    glUseProgram(_shaderContext); 

    glBindVertexArray(_vaoContext);

    //Pass game time to shader
    glUniform1f(_timeLocation, seconds);

    auto lightMVP = light->getLightMVP();
    auto cameraMVP = light->getCameraMVP();

    //Pass far plane to shader
    auto projMatrix = cameraMVP.getProjectionMatrix().getFlatBuffer();
    float nearVal = (2.0f*projMatrix[11]) / (2.0f*projMatrix[10] - 2.0f);
    float farVal = ((projMatrix[10] - 1.0f)*nearVal) / (projMatrix[10] + 1.0f);
    glUniform1f(_farPlaneLocation, farVal);

    //Pass the type of fire to the shader to simulate i.e. candle light or bon fire
    glUniform1i(_fireTypeLocation, 2);

    //Transform screen space quad to the MVP of the emitting light
    //and setting the rotation matrix to identity for billboarding
    auto viewMatrixPrt = cameraMVP.getViewBuffer();
    auto viewMatrix = Matrix::translation(viewMatrixPrt[3], viewMatrixPrt[7], viewMatrixPrt[11]);
    auto modelMatrixPrt = lightMVP.getModelBuffer();
    auto modelMatrix = Matrix::cameraTranslation(modelMatrixPrt[3], modelMatrixPrt[7], modelMatrixPrt[11]);

    auto viewNoTrans = cameraMVP.getViewMatrix();
    viewNoTrans.getFlatBuffer()[3] = 0.0;
    viewNoTrans.getFlatBuffer()[7] = 0.0;
    viewNoTrans.getFlatBuffer()[11] = 0.0;

    auto modelView = cameraMVP.getViewMatrix() * lightMVP.getModelMatrix();
    glUniformMatrix4fv(_modelViewLocation, 1, GL_TRUE, modelView.getFlatBuffer());
    
    auto projection = cameraMVP.getProjectionMatrix();
    glUniformMatrix4fv(_projectionLocation, 1, GL_TRUE, projection.getFlatBuffer());

    auto inverseViewNoTrans = viewNoTrans.inverse();
    glUniformMatrix4fv(_inverseViewLocation, 1, GL_TRUE, inverseViewNoTrans.getFlatBuffer());

    //screen space quad
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)6);
    
    glBindVertexArray(0);

    glUseProgram(0);//end using this shader
}