#include "ShadowedPointLight.h"

ShadowedPointLight::ShadowedPointLight(ViewEvents* eventWrapper,
    MVP mvp, EffectType effect, Vector4 color) :
    Light(eventWrapper, mvp, LightType::SHADOWED_POINT, effect, color),
    _shadow(2000, 2000){

    std::vector<Sphere>* spheres = new std::vector<Sphere>{ Sphere(getRange(), getPosition()) };
    _vao.createVAO(spheres, GeometryConstruction::LINE_WIREFRAME);
}

void ShadowedPointLight::renderShadow(std::vector<Entity*> entityList) {
    _shadow.render(entityList, this);
}

Texture* ShadowedPointLight::getDepthTexture() {
    return _shadow.getCubeMapTexture();
}

void ShadowedPointLight::render() {
    
    Light::render();
}

void ShadowedPointLight::renderDebug() {

    Vector4 color(1.0, 0.0, 0.0);
    MVP mvp;
    //Model transform to create frustum cube
    mvp.setView(_cameraMVP.getViewMatrix());
    mvp.setProjection(_cameraMVP.getProjectionMatrix());
    _debugShader->runShader(&mvp, &_vao, {}, color.getFlatBuffer(), GeometryConstruction::LINE_WIREFRAME);

}