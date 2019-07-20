#include "DepthFrameBuffer.h"
#include "EngineManager.h"

DepthFrameBuffer::DepthFrameBuffer(uint32_t width,
                                   uint32_t height) :
    _renderTexture(width, height, TextureFormat::DEPTH32_FLOAT) {

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        //Generate a context for the frame buffer
        glGenFramebuffers(1, &_frameBufferContext);

        //Bind the frame buffer context to complete operations on it
        glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferContext);

        //Finally attach the texture to the previously generated frame buffer
        //the texture will be used in later shader texture sampling
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D,
                               _renderTexture.getContext(), 0);

        //Tells opengl that the frame buffer will not have a color buffer
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        //check the frame buffer's health
        GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Frame buffer cannot be generated! Status: " << status << std::endl;
        }

        //remove framebuffer context
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

DepthFrameBuffer::~DepthFrameBuffer() {

}

GLuint DepthFrameBuffer::getFrameBufferContext() {
    return _frameBufferContext;
}
GLuint DepthFrameBuffer::getTextureContext() {
    return _renderTexture.getContext();
}
GLuint DepthFrameBuffer::getWidth() {
    return _renderTexture.getWidth();
}
GLuint DepthFrameBuffer::getHeight() {
    return _renderTexture.getHeight();
}

Texture* DepthFrameBuffer::getTexture() {
    return &_renderTexture;
}
