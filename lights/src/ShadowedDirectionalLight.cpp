#include "ShadowedDirectionalLight.h"

ShadowedDirectionalLight::ShadowedDirectionalLight(ViewManagerEvents* eventWrapper,
    MVP mvp, EffectType effect, Vector4 color) :
    Light(eventWrapper, mvp, LightType::SHADOWED_DIRECTIONAL, effect, color),
    _shadow(static_cast<unsigned int>(getWidth()),
            static_cast<unsigned int>(getHeight())) {

}


void ShadowedDirectionalLight::renderShadow(std::vector<Entity*> entityList) {
    _shadow.render(entityList, this);
}

GLuint ShadowedDirectionalLight::getDepthTexture() {
    return _shadow.getTextureContext();
}

void ShadowedDirectionalLight::render() {

    if (_type == LightType::DIRECTIONAL || _type == LightType::SHADOWED_DIRECTIONAL) {
        Vector4 color(1.0, 0.0, 0.0);
        _debugShader->runShader(&getLightMVP(), &_vao, {}, color.getFlatBuffer());
    }

    Light::render();
}