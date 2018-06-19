#include "EffectShader.h"
#include "Light.h"
#include "Water.h"
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
    _fireColorLocation = glGetUniformLocation(_shaderContext, "fireColor");

    if (shaderName == "waterShader") {
        _normalLocation = glGetUniformLocation(_shaderContext, "normal");
        _noiseTextureLocation = glGetUniformLocation(_shaderContext, "noiseTexture");
    }

}

EffectShader::~EffectShader() {

}

void EffectShader::runShader(Effect* effectObject, float seconds) {

    //LOAD IN SHADER
    glUseProgram(_shaderContext);

    glBindVertexArray(_vaoContext);


    if (effectObject->getType() == EffectType::Fire) {

        //Pass game time to shader
        glUniform1f(_timeLocation, seconds);

        Light* light = static_cast<Light*>(effectObject);
        auto lightMVP = light->getLightMVP();
        auto cameraMVP = light->getCameraMVP();

        //Pass far plane to shader
        auto projMatrix = cameraMVP.getProjectionMatrix().getFlatBuffer();
        float nearVal = (2.0f*projMatrix[11]) / (2.0f*projMatrix[10] - 2.0f);
        float farVal = ((projMatrix[10] - 1.0f)*nearVal) / (projMatrix[10] + 1.0f);
        glUniform1f(_farPlaneLocation, farVal);

        //Pass the type of fire to the shader to simulate i.e. candle light or bon fire
        glUniform1i(_fireTypeLocation, 2);

        float* color = light->getColor().getFlatBuffer();
        glUniform3f(_fireColorLocation, color[0], color[1], color[2]);

        auto viewNoTrans = cameraMVP.getViewMatrix();
        viewNoTrans.getFlatBuffer()[3] = 0.0;
        viewNoTrans.getFlatBuffer()[7] = 0.0;
        viewNoTrans.getFlatBuffer()[11] = 0.0;

        auto modelView = cameraMVP.getViewMatrix() * lightMVP.getModelMatrix() * Matrix::scale(2.0f);
        glUniformMatrix4fv(_modelViewLocation, 1, GL_TRUE, modelView.getFlatBuffer());

        auto projection = cameraMVP.getProjectionMatrix();
        glUniformMatrix4fv(_projectionLocation, 1, GL_TRUE, projection.getFlatBuffer());

        auto inverseViewNoTrans = viewNoTrans.inverse();
        glUniformMatrix4fv(_inverseViewLocation, 1, GL_TRUE, inverseViewNoTrans.getFlatBuffer());
    }
    else if (effectObject->getType() == EffectType::Smoke) {

        //Pass game time to shader
        glUniform1f(_timeLocation, seconds/4.0f);

        Light* light = static_cast<Light*>(effectObject);
        auto lightMVP = light->getLightMVP();
        auto cameraMVP = light->getCameraMVP();

        //Pass far plane to shader
        auto projMatrix = cameraMVP.getProjectionMatrix().getFlatBuffer();
        float nearVal = (2.0f*projMatrix[11]) / (2.0f*projMatrix[10] - 2.0f);
        float farVal = ((projMatrix[10] - 1.0f)*nearVal) / (projMatrix[10] + 1.0f);
        glUniform1f(_farPlaneLocation, farVal);

        //Pass the type of fire to the shader to simulate i.e. candle light or bon fire
        glUniform1i(_fireTypeLocation, 2);

        float* color = light->getColor().getFlatBuffer();
        glUniform3f(_fireColorLocation, color[0], color[1], color[2]);

        auto viewNoTrans = cameraMVP.getViewMatrix();
        viewNoTrans.getFlatBuffer()[3] = 0.0;
        viewNoTrans.getFlatBuffer()[7] = 0.0;
        viewNoTrans.getFlatBuffer()[11] = 0.0;

        auto modelView = cameraMVP.getViewMatrix() * lightMVP.getModelMatrix() * Matrix::scale(20.0f);
        glUniformMatrix4fv(_modelViewLocation, 1, GL_TRUE, modelView.getFlatBuffer());

        auto projection = cameraMVP.getProjectionMatrix();
        glUniformMatrix4fv(_projectionLocation, 1, GL_TRUE, projection.getFlatBuffer());

        auto inverseViewNoTrans = viewNoTrans.inverse();
        glUniformMatrix4fv(_inverseViewLocation, 1, GL_TRUE, inverseViewNoTrans.getFlatBuffer());
    }
    else if(effectObject->getType() == EffectType::Water) {

        //Pass game time to shader
        glUniform1f(_timeLocation, seconds);

        Water* water = static_cast<Water*>(effectObject);
        auto cameraMVP = water->getCameraMVP();

        //Pass far plane to shader
        auto projMatrix = cameraMVP.getProjectionMatrix().getFlatBuffer();
        float nearVal = (2.0f*projMatrix[11]) / (2.0f*projMatrix[10] - 2.0f);
        float farVal = ((projMatrix[10] - 1.0f)*nearVal) / (projMatrix[10] + 1.0f);
        glUniform1f(_farPlaneLocation, farVal);

        auto viewNoTrans = cameraMVP.getViewMatrix();
        viewNoTrans.getFlatBuffer()[3] = 0.0;
        viewNoTrans.getFlatBuffer()[7] = 0.0;
        viewNoTrans.getFlatBuffer()[11] = 0.0;

        auto modelView = cameraMVP.getViewMatrix() * Matrix::cameraRotationAroundX(90.0f)
           * Matrix::translation(20.0f, -40.0f, -1.0f)
            * Matrix::scale(100.0f); //add to z component of translation to lower water line.

        glUniformMatrix4fv(_modelViewLocation, 1, GL_TRUE, modelView.getFlatBuffer());

        auto projection = cameraMVP.getProjectionMatrix();
        glUniformMatrix4fv(_projectionLocation, 1, GL_TRUE, projection.getFlatBuffer());

        auto normalMatrix = modelView.inverse().transpose();
        //glUniform mat4 normal matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        glUniformMatrix4fv(_normalLocation, 1, GL_TRUE, normalMatrix.getFlatBuffer());

        glUniform1i(_noiseTextureLocation, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, water->getNoiseTexture());
    }

    //screen space quad
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)6);

    glBindVertexArray(0);

    glUseProgram(0);//end using this shader
}
