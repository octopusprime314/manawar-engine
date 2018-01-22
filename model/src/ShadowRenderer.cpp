#include "ShadowRenderer.h"

ShadowRenderer::ShadowRenderer() : _staticShadowShader("staticShadowShader"),
	_animatedShadowShader("animatedShadowShader") {

}

ShadowRenderer::~ShadowRenderer() {

}

GLuint ShadowRenderer::getDepthTexture() {
    return _shadowFBO.getTextureContext();
}

void ShadowRenderer::generateShadowBuffer(std::vector<Model*> modelList, std::vector<Light*>& lights) {

    //Bind frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, _shadowFBO.getFrameBufferContext());

    //Clear color buffer from frame buffer otherwise framebuffer will contain data from the last draw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);

    // Specify what to render an start acquiring
    GLenum buffers[] = { GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, buffers);

    for (Light* light : lights) {

        for (Model* model : modelList) {

            if (model->getClassType() == ModelClass::ModelType) {
				_staticShadowShader.runShader(model, light);
            }
            else {
				_animatedShadowShader.runShader(model, light);
            }
        }
    }

    //remove framebuffer context
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}