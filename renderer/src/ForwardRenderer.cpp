#include "ForwardRenderer.h"
#include "Model.h"
#include "ShaderBroker.h"
#include "Entity.h"
#include "EngineManager.h"
#include "HLSLShader.h"

ForwardRenderer::ForwardRenderer() :
    _forwardShader(static_cast<ForwardShader*>(ShaderBroker::instance()->getShader("forwardShader"))),
    _instancedForwardShader(static_cast<InstancedForwardShader*>(ShaderBroker::instance()->getShader("instancedShader"))) {

}

ForwardRenderer::~ForwardRenderer() {

}

void ForwardRenderer::forwardLighting(std::vector<Entity*>& entityList, ViewEventDistributor* viewEventDistributor, 
    std::vector<Light*>& lights) {

    for (auto entity : entityList) {
        auto model = entity->getModel();
        if (!model->getIsInstancedModel()) {
            _forwardShader->runShader(entity, viewEventDistributor, lights) ;
        }
        else {
            _instancedForwardShader->runShader(entity, viewEventDistributor, lights);
        }
    }
}
