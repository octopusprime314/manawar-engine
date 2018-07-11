#include "ShadowRenderer.h"
#include "ViewManager.h"
#include "Entity.h"

static const float SHADOW_RESOLUTION = 20.0; //10 pixels per unit distance of 1

ShadowRenderer::ShadowRenderer(Light* sunLightCam, Light* sunLightMap) :
    _staticRendered(false),
    _staticShadowFBO(sunLightCam->getWidth() * SHADOW_RESOLUTION, sunLightCam->getHeight() * SHADOW_RESOLUTION),

    _animatedShadowFBO(sunLightCam->getWidth() * SHADOW_RESOLUTION, sunLightCam->getHeight() * SHADOW_RESOLUTION),

    _mapShadowFBO(sunLightMap->getWidth() * SHADOW_RESOLUTION * (sunLightCam->getWidth() / sunLightMap->getWidth()), 
        sunLightMap->getHeight() * SHADOW_RESOLUTION * (sunLightCam->getHeight() / sunLightMap->getHeight())),

    _staticShadowShader(static_cast<ShadowStaticShader*>(ShaderBroker::instance()->getShader("staticShadowShader"))),
    _animatedShadowShader(static_cast<ShadowAnimatedShader*>(ShaderBroker::instance()->getShader("animatedShadowShader"))) {
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

void ShadowRenderer::generateShadowBuffer(std::vector<Entity*> entityList, std::vector<Light*>& lights) {

    // Specify what to render an start acquiring
    GLenum buffers[] = { GL_NONE };
    Light* light = nullptr;

    //Bind frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, _mapShadowFBO.getFrameBufferContext());

    //Need to change viewport to the resolution of the shadow texture
    glViewport(0, 0, _mapShadowFBO.getWidth(), _mapShadowFBO.getHeight());

    //Clear depth buffer
    glClear(GL_DEPTH_BUFFER_BIT);

    glDrawBuffers(1, buffers);

    light = lights[1];
    for (Entity* entity : entityList) {

        if (entity->getModel()->getClassType() == ModelClass::ModelType) {
            _staticShadowShader->runShader(entity, light);
        }
    }

    //remove framebuffer context
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    _staticRendered = true;

    //Bind frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, _staticShadowFBO.getFrameBufferContext());

    //Need to change viewport to the resolution of the shadow texture
    glViewport(0, 0, _staticShadowFBO.getWidth(), _staticShadowFBO.getHeight());

    //Clear depth buffer
    glClear(GL_DEPTH_BUFFER_BIT);

    glDrawBuffers(1, buffers);

    light = lights[0];
    for (Entity* entity : entityList) {

        if (entity->getModel()->getClassType() == ModelClass::ModelType) {
            _staticShadowShader->runShader(entity, light);
        }
        else if (entity->getModel()->getClassType() == ModelClass::AnimatedModelType) {
            _animatedShadowShader->runShader(entity, light);
        }
    }

    //remove framebuffer context
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Bring to original rendering viewport
    glViewport(0, 0, screenPixelWidth, screenPixelHeight);
}
