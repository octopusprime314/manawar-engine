#include "MRTFrameBuffer.h"

MRTFrameBuffer::MRTFrameBuffer(int colorAttachments) {

    //Create the frame buffer texture context
    GLuint* temp = new GLuint[colorAttachments];
    glGenTextures(colorAttachments, temp);
    _fbTextureContexts.insert(_fbTextureContexts.end(), temp, temp + colorAttachments);
    delete temp;

    //Generate a context for the frame buffer
    glGenFramebuffers(1, &_frameBufferContext);

    //Bind the frame buffer context to complete operations on it
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferContext);

    //Bind current texture context diffuse
    glBindTexture(GL_TEXTURE_2D, _fbTextureContexts[0]); 

    //spell out texture format, 1920 x 1080 texture, RGBA format but can use RGB, data pointer is null 
    //because the frame buffer is responsible for allocating and populating texture data
    //Diffuse data will be in unsigned format with 8 bits each for RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenPixelWidth, screenPixelHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    //texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //remove texture context
    glBindTexture(GL_TEXTURE_2D, 0);

    //Finally attach the texture to the previously generated frame buffer
    //the texture will be used in later shader texture sampling
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 0, GL_TEXTURE_2D, _fbTextureContexts[0], 0);

    //Bind current texture context normal
    glBindTexture(GL_TEXTURE_2D, _fbTextureContexts[1]);

    //spell out texture format, 1920 x 1080 texture, RGB format but can use RGB, data pointer is null 
    //because the frame buffer is responsible for allocating and populating texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, screenPixelWidth, screenPixelHeight, 0, GL_RGBA, GL_FLOAT, NULL);

    //texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //remove texture context
    glBindTexture(GL_TEXTURE_2D, 0);

    //Finally attach the texture to the previously generated frame buffer
    //the texture will be used in later shader texture sampling
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 1, GL_TEXTURE_2D, _fbTextureContexts[1], 0);

    //Bind current texture context normal
    glBindTexture(GL_TEXTURE_2D, _fbTextureContexts[2]);

    //spell out texture format, 1920 x 1080 texture, RGB format but can use RGB, data pointer is null 
    //because the frame buffer is responsible for allocating and populating texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, screenPixelWidth, screenPixelHeight, 0, GL_RGBA, GL_FLOAT, NULL);

    //texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //remove texture context
    glBindTexture(GL_TEXTURE_2D, 0);

    //Finally attach the texture to the previously generated frame buffer
    //the texture will be used in later shader texture sampling
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 2, GL_TEXTURE_2D, _fbTextureContexts[2], 0);

    //Needs to attach a depth render buffer to the frame buffer object!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //I got hosed on this super hard!!!!!!!!!!!!!!!!!!!!!!!!!
    GLuint depth_rb;
    glGenRenderbuffers(1, &depth_rb);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_rb);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, screenPixelWidth, screenPixelHeight);
        
    //Attach depth buffer to FBO
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rb);
    
    //check the frame buffer's health
    GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Frame buffer cannot be generated! Status: " << status << std::endl;
    }

    //remove framebuffer context
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

MRTFrameBuffer::~MRTFrameBuffer() {

}

GLuint MRTFrameBuffer::getFrameBufferContext() {
    return _frameBufferContext;
}

std::vector<GLuint> MRTFrameBuffer::getTextureContexts() {
    return _fbTextureContexts;
}