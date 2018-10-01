#include "DeferredRenderer.h"
#include "Model.h"
#include "SSAO.h"
#include "ShaderBroker.h"
#include "EngineManager.h"
#include "DXLayer.h"

DeferredRenderer::DeferredRenderer() : 
    _mrtFBO(), 
    _deferredShader(static_cast<DeferredShader*>(ShaderBroker::instance()->getShader("deferredShader"))) {

}

DeferredRenderer::~DeferredRenderer() {

}

void DeferredRenderer::deferredLighting(std::vector<Light*>& lights, ViewEventDistributor* viewEventDistributor,
    SSAO* ssao, EnvironmentMap* environmentMap) {
    _deferredShader->runShader(lights, viewEventDistributor, _mrtFBO, ssao, environmentMap);
}

void DeferredRenderer::bind() {

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::DX12) {
        auto presentTarget = DXLayer::instance()->getPresentTarget();
        auto device = DXLayer::instance()->getDevice();
        auto cmdList = DXLayer::instance()->getCmdList();
        presentTarget->bindTarget(device, cmdList, DXLayer::instance()->getCmdListIndex());

    }
    else {
        //Bind frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, _mrtFBO.getFrameBufferContext());

        //Clear color buffer from frame buffer otherwise framebuffer will contain data from the last draw
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Specify what to render an start acquiring
        GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, buffers);
    }
}
void DeferredRenderer::unbind() {
    //unbind frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

MRTFrameBuffer* DeferredRenderer::getGBuffers() {
    return &_mrtFBO;
}
