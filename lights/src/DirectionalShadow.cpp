#include "DirectionalShadow.h"
#include "ShaderBroker.h"
#include "Entity.h"
#include "IOEventDistributor.h"
#include "EngineManager.h"
#include "HLSLShader.h"
#include "ModelBroker.h"
#include "FrustumCuller.h"
#include "GeometryMath.h"

//10 pixels per unit distance of 1
static const float SHADOW_RESOLUTION = 1.0;

DirectionalShadow::DirectionalShadow(GLuint width,
                                     GLuint height) :
    _staticShadowShader(  static_cast<ShadowStaticShader*>(ShaderBroker::instance()->getShader("staticShadowShader"))),

    _animatedShadowShader(static_cast<ShadowAnimatedShader*>(ShaderBroker::instance()->getShader("animatedShadowShader"))),

    _shadow(              static_cast<unsigned int>(width  * SHADOW_RESOLUTION),
                          static_cast<unsigned int>(height * SHADOW_RESOLUTION)) {

}

DirectionalShadow::~DirectionalShadow() {

}

Texture* DirectionalShadow::getTexture() {
    return _shadow.getTexture();
}

void DirectionalShadow::render(std::vector<Entity*> entityList, Light* light) {

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        // Specify what to render an start acquiring
        GLenum buffers[] = { GL_NONE };
        //Bind frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER,
                          _shadow.getFrameBufferContext());

        //Need to change viewport to the resolution of the shadow texture
        glViewport(0,
                   0,
                   _shadow.getWidth(),
                   _shadow.getHeight());

        //Clear depth buffer
        glClear(GL_DEPTH_BUFFER_BIT);

        glDrawBuffers(1, buffers);
    }
    else {
        RenderTexture* depthTexture = static_cast<RenderTexture*>(_shadow.getTexture());
        HLSLShader::setOM({ *depthTexture }, _shadow.getWidth(), _shadow.getHeight());
    }

    Matrix inverseViewProjection = ModelBroker::getViewManager()->getView().inverse() *
                                   ModelBroker::getViewManager()->getProjection().inverse();

    std::vector<Vector4> frustumPlanes;
    GeometryMath::getFrustumPlanes(inverseViewProjection,
                                   frustumPlanes);

    for (Entity* entity : entityList) {

        if (FrustumCuller::getVisibleAABB(entity, frustumPlanes)) {
            if (entity->getModel()->getClassType() == ModelClass::ModelType) {
                _staticShadowShader->runShader(entity, light);
            }
            else if (entity->getModel()->getClassType() == ModelClass::AnimatedModelType) {
                _animatedShadowShader->runShader(entity, light);
            }
        }
    }

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        //Bring to original rendering viewport
        glViewport(0,
                   0,
                   IOEventDistributor::screenPixelWidth,
                   IOEventDistributor::screenPixelHeight);
    }
    else {
        RenderTexture* depthTexture = static_cast<RenderTexture*>(_shadow.getTexture());
        HLSLShader::releaseOM({ *depthTexture });
    }
}
