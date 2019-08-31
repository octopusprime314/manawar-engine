#include "DeferredFrameBuffer.h"
#include "IOEventDistributor.h"
#include "EngineManager.h"

DeferredFrameBuffer::DeferredFrameBuffer() :
    _renderTexture(IOEventDistributor::screenPixelWidth,
                   IOEventDistributor::screenPixelHeight,
                   TextureFormat::RGBA_UNSIGNED_BYTE),
    _debug0Texture(IOEventDistributor::screenPixelWidth,
                   IOEventDistributor::screenPixelHeight,
                   TextureFormat::RGBA_FLOAT),
    _debug1Texture(IOEventDistributor::screenPixelWidth,
                   IOEventDistributor::screenPixelHeight,
                   TextureFormat::RGBA_FLOAT),
    _depthTexture( IOEventDistributor::screenPixelWidth,
                   IOEventDistributor::screenPixelHeight,
                   TextureFormat::DEPTH32_FLOAT) {

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        //Generate a context for the frame buffer
        glGenFramebuffers(1, &_frameBufferContext);

        //Bind the frame buffer context to complete operations on it
        glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferContext);

        //Finally attach the texture to the previously generated frame buffer
        //the texture will be used in later shader texture sampling
        glFramebufferTexture2D(   GL_FRAMEBUFFER,
                                  GL_COLOR_ATTACHMENT0,
                                  GL_TEXTURE_2D,
                                  _renderTexture.getContext(),
                                  0);

        //Needs to attach a depth render buffer to the frame buffer object
        GLuint depthStencilRB;
        glGenRenderbuffers(1, &depthStencilRB);
        glBindRenderbuffer(GL_RENDERBUFFER, depthStencilRB);
        glRenderbufferStorage(    GL_RENDERBUFFER,
                                  GL_DEPTH24_STENCIL8,
                                  _renderTexture.getWidth(),
                                  _renderTexture.getHeight());

        //Attach depth buffer to FBO
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                  GL_DEPTH_ATTACHMENT,
                                  GL_RENDERBUFFER,
                                  depthStencilRB);
        //Attach stencil buffer to FBO
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                  GL_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER,
                                  depthStencilRB);

        //check the frame buffer's health
        GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Frame buffer cannot be generated! Status: " << status << std::endl;
        }

        //remove framebuffer context
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

DeferredFrameBuffer::~DeferredFrameBuffer() {

}

GLuint DeferredFrameBuffer::getFrameBufferContext() {
    return _frameBufferContext;
}

GLuint DeferredFrameBuffer::getTextureContext() {
    return _renderTexture.getContext();
}

Texture* DeferredFrameBuffer::getRenderTexture() {
    return &_renderTexture;
}
Texture* DeferredFrameBuffer::getDepthTexture() {
    return &_depthTexture;
}
Texture* DeferredFrameBuffer::getDebug0Texture() {
    return &_debug0Texture;
}
Texture* DeferredFrameBuffer::getDebug1Texture() {
    return &_debug1Texture;
}
