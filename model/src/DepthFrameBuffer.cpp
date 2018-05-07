#include "DepthFrameBuffer.h"

DepthFrameBuffer::DepthFrameBuffer(unsigned int width, unsigned int height) :
    _width(width), _height(height) {

    //Create the frame buffer texture context
    glGenTextures(1, &_fbTextureContext);

    //Bind current texture context
    glBindTexture(GL_TEXTURE_2D, _fbTextureContext);

    //spell out texture format, width x height texture, Depth 32 bit format, data pointer is null 
    //because the frame buffer is responsible for allocating and populating texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    //texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //remove texture context
    glBindTexture(GL_TEXTURE_2D, 0);

    //Generate a context for the frame buffer
    glGenFramebuffers(1, &_frameBufferContext);

    //Bind the frame buffer context to complete operations on it
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferContext);

    //Finally attach the texture to the previously generated frame buffer
    //the texture will be used in later shader texture sampling
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _fbTextureContext, 0);

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

DepthFrameBuffer::~DepthFrameBuffer() {

}

GLuint DepthFrameBuffer::getFrameBufferContext() {
    return _frameBufferContext;
}
GLuint DepthFrameBuffer::getTextureContext() {
    return _fbTextureContext;
}
GLuint DepthFrameBuffer::getWidth() {
    return _width;
}
GLuint DepthFrameBuffer::getHeight() {
    return _height;
}