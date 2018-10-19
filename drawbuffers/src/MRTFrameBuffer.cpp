#include "MRTFrameBuffer.h"
#include "IOEventDistributor.h"
#include "EngineManager.h"
#include "DXLayer.h"

MRTFrameBuffer::MRTFrameBuffer() {

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        //Generate a context for the frame buffer
        glGenFramebuffers(1, &_frameBufferContext);

        //Bind the frame buffer context to complete operations on it
        glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferContext);
    }
    else {

    }

    _gBufferTextures.push_back(RenderTexture(IOEventDistributor::screenPixelWidth, IOEventDistributor::screenPixelHeight, TextureFormat::RGBA_UNSIGNED_BYTE));

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        //Finally attach the texture to the previously generated frame buffer
        //the texture will be used in later shader texture sampling
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _gBufferTextures[0].getContext(), 0);
    }
    else {

    }

    _gBufferTextures.push_back(RenderTexture(IOEventDistributor::screenPixelWidth, IOEventDistributor::screenPixelHeight, TextureFormat::RGBA_FLOAT));

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {

        //Finally attach the texture to the previously generated frame buffer
        //the texture will be used in later shader texture sampling
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _gBufferTextures[1].getContext(), 0);
    }
    else {

    }

    _gBufferTextures.push_back(RenderTexture(IOEventDistributor::screenPixelWidth, IOEventDistributor::screenPixelHeight, TextureFormat::RGBA_FLOAT));

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {

        //Finally attach the texture to the previously generated frame buffer
        //the texture will be used in later shader texture sampling
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _gBufferTextures[2].getContext(), 0);
    }
    else {

    }

    _gBufferTextures.push_back(RenderTexture(IOEventDistributor::screenPixelWidth, IOEventDistributor::screenPixelHeight, TextureFormat::DEPTH32_FLOAT));

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {

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
    }
    else {

    }

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
