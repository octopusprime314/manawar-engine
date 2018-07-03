#include "ForwardRenderer.h"
#include "Model.h"
#include "ShaderBroker.h"

ShaderBroker* ForwardRenderer::_shaderManager = ShaderBroker::instance();

ForwardRenderer::ForwardRenderer() :
    _forwardShader(static_cast<ForwardShader*>(_shaderManager->getShader("forwardShader"))),
    _instancedForwardShader(static_cast<InstancedForwardShader*>(_shaderManager->getShader("instancedShader")))
    /*_forwardShader("forwardShader"),
    _instancedForwardShader("instancedShader")*/ {

}

ForwardRenderer::~ForwardRenderer() {

}

void ForwardRenderer::forwardLighting(std::vector<Model*>& modelList, ViewManager* viewManager, ShadowRenderer* shadowRenderer,
    std::vector<Light*>& lights, PointShadowMap* pointShadowMap) {

    for (auto model : modelList) {

        if (!model->getIsInstancedModel()) {
            _forwardShader->runShader(model, viewManager, shadowRenderer, lights, pointShadowMap);
        }
        else {
            _instancedForwardShader->runShader(model, viewManager, shadowRenderer, lights, pointShadowMap);
        }
    }
}
