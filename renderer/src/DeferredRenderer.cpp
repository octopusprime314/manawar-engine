#include "DeferredRenderer.h"
#include "Model.h"
#include "SSAO.h"
#include "ShaderBroker.h"

DeferredRenderer::DeferredRenderer() : 
    _mrtFBO(), 
    _deferredShader(static_cast<DeferredShader*>(ShaderBroker::instance()->getShader("deferredShader"))) {

}

DeferredRenderer::~DeferredRenderer() {

}

void DeferredRenderer::deferredLighting(std::vector<Light*>& lights, ViewManager* viewManager,
    SSAO* ssao, EnvironmentMap* environmentMap) {
    _deferredShader->runShader(lights, viewManager, _mrtFBO, ssao, environmentMap);
}

void DeferredRenderer::bind() {
    //Bind frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, _mrtFBO.getFrameBufferContext());

    //Clear color buffer from frame buffer otherwise framebuffer will contain data from the last draw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Specify what to render an start acquiring
    GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(4, buffers);
}
void DeferredRenderer::unbind() {
    //unbind frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

MRTFrameBuffer* DeferredRenderer::getGBuffers() {
    return &_mrtFBO;
}
