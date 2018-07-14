#include "ShadowedDirectionalLight.h"

ShadowedDirectionalLight::ShadowedDirectionalLight(ViewManagerEvents* eventWrapper,
    MVP mvp, EffectType effect, Vector4 color) :
    Light(eventWrapper, mvp, LightType::SHADOWED_DIRECTIONAL, effect, color),
    _shadow(static_cast<unsigned int>(getWidth()),
            static_cast<unsigned int>(getHeight())) {


    std::vector<Cube>* cubes = new std::vector<Cube>{ Cube(2.0, 2.0, 2.0, Vector4(0.0, 0.0, 0.0)) };
    _vao.createVAO(cubes, GeometryConstruction::TRIANGLE_MESH);
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

        //Build inverse projection matrix which will properly transform unit cube
        //to the frustum vertices to visualize it in the engine
        MVP lightMVP = getLightMVP();
        MVP mvp;
        //Model transform to create frustum cube
        mvp.setModel(lightMVP.getViewMatrix().inverse() * lightMVP.getProjectionMatrix().inverse());
        mvp.setView(_cameraMVP.getViewMatrix());
        mvp.setProjection(_cameraMVP.getProjectionMatrix());
        _debugShader->runShader(&mvp, &_vao, {}, color.getFlatBuffer(), GeometryConstruction::TRIANGLE_MESH);
    }

    Light::render();
}