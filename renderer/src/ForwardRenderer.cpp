#include "ForwardRenderer.h"
#include "Model.h"
#include "ShaderBroker.h"
#include "Entity.h"

ForwardRenderer::ForwardRenderer() :
    _forwardShader(static_cast<ForwardShader*>(ShaderBroker::instance()->getShader("forwardShader"))),
    _instancedForwardShader(static_cast<InstancedForwardShader*>(ShaderBroker::instance()->getShader("instancedShader"))) {

}

ForwardRenderer::~ForwardRenderer() {

}

void ForwardRenderer::forwardLighting(std::vector<Entity*>& entityList, ViewManager* viewManager, ShadowRenderer* shadowRenderer,
    std::vector<Light*>& lights, PointShadowMap* pointShadowMap) {

    for (auto entity : entityList) {
        auto model = entity->getModel();
        if (!model->getIsInstancedModel()) {
            _forwardShader->runShader(entity, viewManager, shadowRenderer, lights, pointShadowMap);
        }
        else {
            _instancedForwardShader->runShader(entity, viewManager, shadowRenderer, lights, pointShadowMap);
        }
    }
}
