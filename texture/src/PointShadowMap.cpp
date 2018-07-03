#include "PointShadowMap.h"
ShaderBroker* PointShadowMap::_shaderManager = ShaderBroker::instance();

PointShadowMap::PointShadowMap(GLuint width, GLuint height) :
    CubeMapRenderer(width, height, true),
    _pointShadowShader(static_cast<ShadowPointShader*>(_shaderManager->getShader("pointShadowShader"))),
    _pointAnimatedShadowShader(static_cast<ShadowAnimatedPointShader*>(_shaderManager->getShader("pointAnimatedShadowShader"))) {

}

PointShadowMap::~PointShadowMap() {

}


void PointShadowMap::render(std::vector<Model*> modelList, Light* light) {

    //Does this light support cube map depth rendering
    if (light->getType() == LightType::POINT && light->isShadowCaster()) {

        //Prepare cube face transforms
        preCubeFaceRender(modelList, &light->getLightMVP());

        for (Model* model : modelList) {
            if (model->getClassType() == ModelClass::ModelType) {
                _pointShadowShader->runShader(model, light, _transforms);
            }
            else if (model->getClassType() == ModelClass::AnimatedModelType) {
                _pointAnimatedShadowShader->runShader(model, light, _transforms);
            }
        }
        //Clean up cube face render contexts
        postCubeFaceRender();
    }
}
