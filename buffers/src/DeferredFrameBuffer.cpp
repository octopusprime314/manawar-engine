#include "DeferredFrameBuffer.h"
#include "IOEventDistributor.h"

DeferredFrameBuffer::DeferredFrameBuffer() :
    _renderTexture(IOEventDistributor::screenPixelWidth, IOEventDistributor::screenPixelHeight, TextureFormat::RGBA_UNSIGNED_BYTE) {

    //Generate a context for the frame buffer
    glGenFramebuffers(1, &_frameBufferContext);

    //Bind the frame buffer context to complete operations on it
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferContext);

    //Finally attach the texture to the previously generated frame buffer
    //the texture will be used in later shader texture sampling
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _renderTexture.getContext(), 0);

    //Needs to attach a depth render buffer to the frame buffer object!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //I got hosed on this super hard!!!!!!!!!!!!!!!!!!!!!!!!!
    GLuint depth_stencil_rb;
    glGenRenderbuffers(1, &depth_stencil_rb);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_stencil_rb);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _renderTexture.getWidth(), _renderTexture.getHeight());

    //Attach depth buffer to FBO
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_stencil_rb);
    //Attach stencil buffer to FBO
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_stencil_rb);

    //check the frame buffer's health
    GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Frame buffer cannot be generated! Status: " << status << std::endl;
    }

    //remove framebuffer context
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

DeferredFrameBuffer::~DeferredFrameBuffer() {

}

GLuint DeferredFrameBuffer::getFrameBufferContext() {
    return _frameBufferContext;
}

GLuint DeferredFrameBuffer::getTextureContext() {
    return _renderTexture.getContext();
}

Texture* DeferredFrameBuffer::getTexture() {
    return &_renderTexture;
}
