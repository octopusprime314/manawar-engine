#include "ShadowRenderer.h"

ShadowRenderer::ShadowRenderer() : _staticShadowShader("staticShadowShader"),
	_animatedShadowShader("animatedShadowShader"),
    _staticRendered(false) {

}

ShadowRenderer::~ShadowRenderer() {

}

GLuint ShadowRenderer::getStaticDepthTexture() {
    return _staticShadowFBO.getTextureContext();
}

GLuint ShadowRenderer::getAnimatedDepthTexture() {
    return _animatedShadowFBO.getTextureContext();
}

GLuint ShadowRenderer::getMapDepthTexture() {
    return _mapShadowFBO.getTextureContext();
}

void ShadowRenderer::generateShadowBuffer(std::vector<Model*> modelList, std::vector<Light*>& lights) {

    // Specify what to render an start acquiring
    GLenum buffers[] = { GL_COLOR_ATTACHMENT0};
    Light* light = nullptr;

    //Only render static objects once to generate depth texture
    if(!_staticRendered){
        //Bind frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, _staticShadowFBO.getFrameBufferContext());

        //Clear color buffer from frame buffer otherwise framebuffer will contain data from the last draw
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 0.0);

        glDrawBuffers(1, buffers);

        light = lights[0];
        //for (Light* light : lights) {

            for (Model* model : modelList) {

                if (model->getClassType() == ModelClass::ModelType) {
				    _staticShadowShader.runShader(model, light);
                }
            }
        //}

        //remove framebuffer context
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //Bind frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, _mapShadowFBO.getFrameBufferContext());

        //Clear color buffer from frame buffer otherwise framebuffer will contain data from the last draw
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 0.0);

        glDrawBuffers(1, buffers);

        light = lights[1];
        //for (Light* light : lights) {

            for (Model* model : modelList) {

                if (model->getClassType() == ModelClass::ModelType) {
				    _staticShadowShader.runShader(model, light);
                }
            }
        //}

        //remove framebuffer context
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        _staticRendered = true;

    }

    //Bind frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, _animatedShadowFBO.getFrameBufferContext());

    //Clear color buffer from frame buffer otherwise framebuffer will contain data from the last draw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);

    // Specify what to render an start acquiring
    glDrawBuffers(1, buffers);

    light = lights[0];
    //for (Light* light : lights) {

        for (Model* model : modelList) {

            if (model->getClassType() == ModelClass::AnimatedModelType) {
				_animatedShadowShader.runShader(model, light);
            }
        }
    //}

    //remove framebuffer context
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}