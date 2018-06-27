#include "EffectShader.h"
#include "Light.h"
#include "Water.h"
#include "ViewManager.h"

EffectShader::EffectShader(std::string shaderName) : Shader(shaderName) {

    glGenVertexArrays(1, &_vaoContext);
}

EffectShader::~EffectShader() {

}

void EffectShader::runShader(Effect* effectObject, float seconds) {

    //LOAD IN SHADER
    glUseProgram(_shaderContext);
    glBindVertexArray(_vaoContext);
    
    auto cameraMVP = effectObject->getCameraMVP();

    auto view = cameraMVP.getViewMatrix();
    updateUniform("view", view.getFlatBuffer());

    auto projection = cameraMVP.getProjectionMatrix();
    updateUniform("projection", projection.getFlatBuffer());

    if (effectObject->getType() == EffectType::Fire || effectObject->getType() == EffectType::Smoke) {

        Light* light = static_cast<Light*>(effectObject);
       
        auto planeScale = Matrix::scale(2.0f);
        if (effectObject->getType() == EffectType::Smoke) {
            seconds /= 4.0f;
            planeScale = Matrix::scale(20.0f);
        }
        auto lightMVP = light->getLightMVP();

        //Pass the type of fire to the shader to simulate i.e. candle light or bon fire
        int fireType = 2;
        updateUniform("fireType", &fireType);

        float* color = light->getColor().getFlatBuffer();
        updateUniform("fireColor", color);

        auto viewNoTrans = cameraMVP.getViewMatrix();
        viewNoTrans.getFlatBuffer()[3] = 0.0;
        viewNoTrans.getFlatBuffer()[7] = 0.0;
        viewNoTrans.getFlatBuffer()[11] = 0.0;

        auto model = lightMVP.getModelMatrix() * planeScale;
        updateUniform("model", model.getFlatBuffer());

        auto inverseViewNoTrans = viewNoTrans.inverse();
        updateUniform("inverseViewNoTrans", inverseViewNoTrans.getFlatBuffer());

        //Pass far plane to shader
        auto projMatrix = cameraMVP.getProjectionMatrix().getFlatBuffer();
        float nearVal = (2.0f*projMatrix[11]) / (2.0f*projMatrix[10] - 2.0f);
        float farVal = ((projMatrix[10] - 1.0f)*nearVal) / (projMatrix[10] + 1.0f);
        updateUniform("farPlane", &farVal);
    }
    else if (effectObject->getType() == EffectType::Water) {

        Water* water = static_cast<Water*>(effectObject);

        auto model = Matrix::cameraRotationAroundX(90.0f)
            * Matrix::translation(20.0f, -40.0f, -1.0f)
            * Matrix::scale(300.0f); //add to z component of translation to lower water line.

        updateUniform("model", model.getFlatBuffer());

        auto normalMatrix = (view * model).inverse().transpose();
        updateUniform("normal", normalMatrix.getFlatBuffer());

        updateUniform("noiseTexture", GL_TEXTURE0, water->getNoiseTexture());
    }

    //Pass game time to shader
    updateUniform("time", &seconds);

    //screen space quad
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)4);
    glBindVertexArray(0);
    glUseProgram(0);//end using this shader
}