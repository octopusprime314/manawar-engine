#include "FrameBuffer.h"

FrameBuffer::FrameBuffer() {

    //Create the frame buffer texture context
    glGenTextures(1, &_fbTextureContext);

    //Bind current texture context
    glBindTexture(GL_TEXTURE_2D, _fbTextureContext);

    //spell out texture format, 1920 x 1080 texture, RGBA format but can use RGB, data pointer is null 
    //because the frame buffer is responsible for allocating and populating texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

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
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fbTextureContext, 0);

    //check the frame buffer's health
    GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Frame buffer cannot be generated! Status: " << status << std::endl;
    }

    //remove framebuffer context
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer() {

}

GLuint FrameBuffer::getFrameBufferContext() {
    return _frameBufferContext;
}

GLuint FrameBuffer::getTextureContext() {
    return _fbTextureContext;
}