#include "ForwardRenderer.h"
#include "Model.h"
#include "ShaderBroker.h"
#include "Entity.h"
#include "EngineManager.h"
#include "HLSLShader.h"
#include "FrustumCuller.h"
#include "ModelBroker.h"
#include "ViewEventDistributor.h"

ForwardRenderer::ForwardRenderer() :
    _forwardShader(static_cast<ForwardShader*>(ShaderBroker::instance()->getShader("forwardShader"))),
    _instancedForwardShader(static_cast<InstancedForwardShader*>(ShaderBroker::instance()->getShader("instancedShader"))) {

}

ForwardRenderer::~ForwardRenderer() {

}

void ForwardRenderer::forwardLighting(std::vector<Entity*>& entityList, ViewEventDistributor* viewEventDistributor, 
    std::vector<Light*>& lights) {

    Matrix inverseViewProjection = ModelBroker::getViewManager()->getView().inverse() *
                                   ModelBroker::getViewManager()->getProjection().inverse();

    for (auto entity : entityList) {

        //if (FrustumCuller::getVisibleAABB(entity, inverseViewProjection)) {
            auto model = entity->getModel();
            if (!model->getIsInstancedModel()) {
                _forwardShader->runShader(entity, viewEventDistributor, lights);
            }
            else {
                _instancedForwardShader->runShader(entity, viewEventDistributor, lights);
            }
        //}
    }
}
