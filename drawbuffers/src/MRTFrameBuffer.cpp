#include "MRTFrameBuffer.h"
#include "IOEventDistributor.h"

MRTFrameBuffer::MRTFrameBuffer() {

    //Generate a context for the frame buffer
    glGenFramebuffers(1, &_frameBufferContext);

    //Bind the frame buffer context to complete operations on it
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferContext);

    _gBufferTextures.push_back(RenderTexture(IOEventDistributor::screenPixelWidth, IOEventDistributor::screenPixelHeight, TextureFormat::RGBA_UNSIGNED_BYTE));

    //Finally attach the texture to the previously generated frame buffer
    //the texture will be used in later shader texture sampling
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _gBufferTextures[0].getContext(), 0);

    _gBufferTextures.push_back(RenderTexture(IOEventDistributor::screenPixelWidth, IOEventDistributor::screenPixelHeight, TextureFormat::RGBA_FLOAT));

    //Finally attach the texture to the previously generated frame buffer
    //the texture will be used in later shader texture sampling
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _gBufferTextures[1].getContext(), 0);

    _gBufferTextures.push_back(RenderTexture(IOEventDistributor::screenPixelWidth, IOEventDistributor::screenPixelHeight, TextureFormat::RGBA_FLOAT));

    //Finally attach the texture to the previously generated frame buffer
    //the texture will be used in later shader texture sampling
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _gBufferTextures[2].getContext(), 0);

    _gBufferTextures.push_back(RenderTexture(IOEventDistributor::screenPixelWidth, IOEventDistributor::screenPixelHeight, TextureFormat::DEPTH32_FLOAT));

    //Finally attach the texture to the previously generated frame buffer
    //the texture will be used in later shader texture sampling
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _gBufferTextures[3].getContext(), 0);

    //check the frame buffer's health
    GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Frame buffer cannot be generated! Status: " << status << std::endl;
    }

    //remove framebuffer context
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    for (auto& texture : _gBufferTextures) {
        _fbTextureContexts.push_back(texture.getContext());
    }
}

MRTFrameBuffer::~MRTFrameBuffer() {

}

GLuint MRTFrameBuffer::getFrameBufferContext() {
    return _frameBufferContext;
}

std::vector<GLuint> MRTFrameBuffer::getTextureContexts() {
    return _fbTextureContexts;
}

std::vector<RenderTexture>& MRTFrameBuffer::getTextures() {
    return _gBufferTextures;
}
