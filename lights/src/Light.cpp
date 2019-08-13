#include "Light.h"
#include "MasterClock.h"
#include <random>
#include "ShaderBroker.h"

Light::Light(ViewEvents* eventWrapper,
             MVP         mvp,
             LightType   type,
             EffectType  effect,
             Vector4     color) :

    Effect(eventWrapper,
           "fireShader",
           effect),

    _type           (type),
    _lightMVP       (mvp),
    _color          (color),
    _milliSecondTime(0) {

    _debugShader = static_cast<DebugShader*>(ShaderBroker::instance()->getShader("debugShader"));
    
    MasterClock::instance()->subscribeKinematicsRate(std::bind(&Light::_updateTime,
                                                               this,
                                                               std::placeholders::_1));

    //Extract light position from view matrix
    float* inverseView = _lightMVP.getViewMatrix().inverse().getFlatBuffer();
    _position          = Vector4(inverseView[3],
                                 inverseView[7],
                                 inverseView[11],
                                 1.0);

}

MVP Light::getLightMVP() {

    //Move the positions of the lights based on the camera view except
    //the large map directional light that is used for low resolution
    //shadow map generation
    if (_type == LightType::SHADOWED_DIRECTIONAL ||
        _type == LightType::DIRECTIONAL) {
        return _lightMVP;
    }
    else {
        return _lightMVP;
    }
}

MVP Light::getCameraMVP() {
    return _cameraMVP;
}

Vector4 Light::getPosition() {

    //Extract light position from model matrix
    float* inverseModel = _lightMVP.getModelMatrix().getFlatBuffer();
    _position           = Vector4(inverseModel[3],
                                  inverseModel[7],
                                  inverseModel[11],
                                  1.0);
    return _position;
}

Vector4 Light::getLightDirection() {
    auto    viewMatrix = _lightMVP.getViewMatrix().getFlatBuffer();
    Vector4 lightDirection(-viewMatrix[1],
                           -viewMatrix[5],
                           -viewMatrix[9]);
    return lightDirection;
}


LightType Light::getType() {
    return _type;
}

Vector4& Light::getColor() {
    return _color;
}

bool Light::isShadowCaster() {
    
    if (_type == LightType::SHADOWED_DIRECTIONAL ||
        _type == LightType::SHADOWED_POINT       ||
        _type == LightType::SHADOWED_SPOTLIGHT) {
        return true;
    }
    return false;
}

float Light::getRange() {

    //Projection
    if (_type == LightType::SHADOWED_POINT     ||
        _type == LightType::SHADOWED_SPOTLIGHT ||
        _type == LightType::POINT              ||
        _type == LightType::SPOTLIGHT) {

        float* projMatrix = _lightMVP.getProjectionBuffer();

        float  nearVal    = (2.0f*projMatrix[10]) /
                            (2.0f*projMatrix[11] - 2.0f);

        float  farVal     = ((projMatrix[11] - 1.0f)*nearVal) /
                            (projMatrix[11] + 1.0f);
        return farVal;
    }
    //Orthographic
    else {
        float* projMatrix = _lightMVP.getProjectionBuffer();
        float farVal    = -(1.0f - projMatrix[11]) /
                            projMatrix[10];
        return farVal;
    }
}

void Light::setMVP(MVP mvp) {
    _lightMVP = mvp;
}
float Light::getWidth() {

    float* projMatrix = _lightMVP.getProjectionBuffer();
    float  width      = 2.0f * (1.0f - projMatrix[3]) / projMatrix[0];
    return width;
}
float Light::getHeight() {

    float* projMatrix = _lightMVP.getProjectionBuffer();
    float  height     = 2.0f * (1.0f + projMatrix[7]) / projMatrix[5];
    return height;
}

void Light::_updateTime(int time) {

    //Every update comes in real time so in order to speed up
    //we need to multiply that value by some constant
    //A full day takes one minute should do it lol
    //divide total time by 60 seconds times 1000 to convert to milliseconds
    uint64_t updateTimeAmplified = dayLengthMilliseconds / (60 * 1000);
    _milliSecondTime             += (updateTimeAmplified*time);
    _milliSecondTime             %= dayLengthMilliseconds;

    //if (_type == LightType::SHADOWED_DIRECTIONAL || _type == LightType::DIRECTIONAL) {

    //    //fraction of the rotation
    //    float posInRotation = static_cast<float>(_milliSecondTime) /
    //                          static_cast<float>(dayLengthMilliseconds);

    //    float* view         = _lightMVP.getViewMatrix().getFlatBuffer();
    //    float radiusOfLight = Vector4(view[3],
    //                                  view[7],
    //                                  view[11],
    //                                  1.0f).getMagnitude();

    //    _lightMVP.setView(Matrix::translation(0.0,
    //                                          0.0,
    //                                          radiusOfLight) *
    //                      Matrix::rotationAroundX(posInRotation * 360.0f));
    //}
}

void Light::render() {

    if (_effectType != EffectType::None) {
        //Bring the time back to real time for the effects shader
        //The amount of milliseconds in 24 hours
        uint64_t updateTimeAmplified  = dayLengthMilliseconds /
                                        (60 * 1000);
        float    realTimeMilliSeconds = static_cast<float>(_milliSecondTime) /
                                        static_cast<float>(updateTimeAmplified);
        _effectShader->runShader(this,
                                 realTimeMilliSeconds / 1000.f);
    }
}

void Light::renderShadow(std::vector<Entity*> entityList) {};