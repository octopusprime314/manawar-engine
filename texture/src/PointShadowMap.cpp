#include "PointShadowMap.h"
#include "ShaderBroker.h"
#include "Entity.h"

PointShadowMap::PointShadowMap(GLuint width, GLuint height) :
    CubeMapRenderer(width, height, true),
    _pointShadowShader(static_cast<ShadowPointShader*>(ShaderBroker::instance()->getShader("pointShadowShader"))),
    _pointAnimatedShadowShader(static_cast<ShadowAnimatedPointShader*>(ShaderBroker::instance()->getShader("pointAnimatedShadowShader"))) {

}

PointShadowMap::~PointShadowMap() {

}


void PointShadowMap::render(std::vector<Entity*> entityList, Light* light) {

    //Does this light support cube map depth rendering
    if (light->getType() == LightType::POINT && light->isShadowCaster()) {

        //Prepare cube face transforms
        preCubeFaceRender(entityList, &light->getLightMVP());

        for (Entity* entity : entityList) {
            if (entity->getModel()->getClassType() == ModelClass::ModelType) {
                _pointShadowShader->runShader(entity, light, _transforms);
            }
            else if (entity->getModel()->getClassType() == ModelClass::AnimatedModelType) {
                _pointAnimatedShadowShader->runShader(entity, light, _transforms);
            }
        }
        //Clean up cube face render contexts
        postCubeFaceRender();
    }
}
