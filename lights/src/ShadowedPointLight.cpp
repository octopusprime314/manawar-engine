#include "ShadowedPointLight.h"

ShadowedPointLight::ShadowedPointLight(ViewManagerEvents* eventWrapper,
    MVP mvp, EffectType effect, Vector4 color) :
    Light(eventWrapper, mvp, LightType::SHADOWED_POINT, effect, color),
    _shadow(2000, 2000){

}

void ShadowedPointLight::renderShadow(std::vector<Entity*> entityList) {
    _shadow.render(entityList, this);
}

GLuint ShadowedPointLight::getDepthTexture() {
    return _shadow.getCubeMapTexture();
}

void ShadowedPointLight::render() {
    //Bring the time back to real time for the effects shader
    //The amount of milliseconds in 24 hours
    const uint64_t dayLengthMilliseconds = 24 * 60 * 60 * 1000;
    uint64_t updateTimeAmplified = dayLengthMilliseconds / (60 * 1000);
    float realTimeMilliSeconds = static_cast<float>(_milliSecondTime) / static_cast<float>(updateTimeAmplified);
    _effectShader->runShader(this, realTimeMilliSeconds / 1000.f);
}