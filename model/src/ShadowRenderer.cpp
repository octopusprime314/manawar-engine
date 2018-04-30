#include "ShadowRenderer.h"
#include "ViewManager.h"

ShadowRenderer::ShadowRenderer(GLuint width, GLuint height) : 
    _staticShadowShader("staticShadowShader"),
	_animatedShadowShader("animatedShadowShader"),
    _staticRendered(false),
    _staticShadowFBO(width*4, height*4),
    _animatedShadowFBO(width*4, height*4),
    _mapShadowFBO(width, height) {

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
    GLenum buffers[] = { GL_DEPTH_ATTACHMENT };
    Light* light = nullptr;

    //Only render static objects once to generate depth texture
    if(!_staticRendered){
  
        //Bind frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, _mapShadowFBO.getFrameBufferContext());

        //Need to change viewport to the resolution of the shadow texture
        glViewport(0, 0, _mapShadowFBO.getWidth(), _mapShadowFBO.getHeight());

        //Clear depth buffer
        glClear(GL_DEPTH_BUFFER_BIT);

        glDrawBuffers(1, buffers);

        light = lights[1];
        for (Model* model : modelList) {

            if (model->getClassType() == ModelClass::ModelType) {
				_staticShadowShader.runShader(model, light);
            }
        }

        //remove framebuffer context
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        _staticRendered = true;

    }

    //Bind frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, _staticShadowFBO.getFrameBufferContext());

    //Need to change viewport to the resolution of the shadow texture
    glViewport(0, 0, _staticShadowFBO.getWidth(), _staticShadowFBO.getHeight());

    //Clear depth buffer
    glClear(GL_DEPTH_BUFFER_BIT);

    glDrawBuffers(1, buffers);

    light = lights[0];
    for (Model* model : modelList) {

        if (model->getClassType() == ModelClass::ModelType) {
            _staticShadowShader.runShader(model, light);
        }
    }

    //remove framebuffer context
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Bind frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, _animatedShadowFBO.getFrameBufferContext());

    //Clear depth buffer
    glClear(GL_DEPTH_BUFFER_BIT);

    //Blit static depth info into animation depth render pass
    glBindFramebuffer(GL_READ_FRAMEBUFFER, _staticShadowFBO.getFrameBufferContext());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _animatedShadowFBO.getFrameBufferContext());

    glBlitFramebuffer(0, 0, _staticShadowFBO.getWidth(), _staticShadowFBO.getHeight(),
                      0, 0, _animatedShadowFBO.getWidth(), _animatedShadowFBO.getHeight(),
                      GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    //Bind frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, _animatedShadowFBO.getFrameBufferContext());

    //Need to change viewport to the resolution of the shadow texture
    glViewport(0, 0, _animatedShadowFBO.getWidth(), _animatedShadowFBO.getHeight());

    // Specify what to render an start acquiring
    glDrawBuffers(1, buffers);

    light = lights[0];
    for (Model* model : modelList) {

        if (model->getClassType() == ModelClass::AnimatedModelType) {
			_animatedShadowShader.runShader(model, light);
        }
    }

    //remove framebuffer context
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Bring to original rendering viewport
    glViewport(0, 0, screenPixelWidth, screenPixelHeight);
}