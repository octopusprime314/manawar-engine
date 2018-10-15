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

void MRTFrameBuffer::bind() {
    
    auto device = DXLayer::instance()->getDevice();
    auto cmdList = DXLayer::instance()->getCmdList();

    D3D12_VIEWPORT viewPort =
    {
        0.0f,
        0.0f,
        static_cast<float>(IOEventDistributor::screenPixelWidth),
        static_cast<float>(IOEventDistributor::screenPixelHeight),
        0.0f,
        1.0f
    };

    // Scissor rectangle

    D3D12_RECT rectScissor = { 0, 0, (LONG)IOEventDistributor::screenPixelWidth,(LONG)IOEventDistributor::screenPixelHeight };

    D3D12_CPU_DESCRIPTOR_HANDLE* handles = new D3D12_CPU_DESCRIPTOR_HANDLE[_gBufferTextures.size() - 1];
    int handleIndex = 0;
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle;
    for (auto buffer : _gBufferTextures) {
        if (buffer.getFormat() == DXGI_FORMAT_D32_FLOAT) {
            dsvHandle = buffer.getHandle();
            buffer.bindTarget(D3D12_RESOURCE_STATE_DEPTH_WRITE);
        }
        else {
            handles[handleIndex++] = buffer.getHandle();
            buffer.bindTarget(D3D12_RESOURCE_STATE_RENDER_TARGET);
        }
    }

    const float clear[] = { 0.0f, 0.0f, 0.0f, 1.000f };

    cmdList->RSSetViewports(1, &viewPort);
    cmdList->RSSetScissorRects(1, &rectScissor);

    cmdList->OMSetRenderTargets(
        static_cast<UINT>(_gBufferTextures.size() - 1),
        handles,
        false,
        &dsvHandle);

    int rtvIndex = 0;
    for (int i = 0; i < _gBufferTextures.size(); i++) {
        
        // Clear target
        if (_gBufferTextures[i].getFormat() == DXGI_FORMAT_D32_FLOAT) {

            cmdList->ClearDepthStencilView(
                dsvHandle,
                D3D12_CLEAR_FLAG_DEPTH,
                1.0f,
                0,
                NULL,
                0
            );
        }
        else {

            cmdList->ClearRenderTargetView(
                handles[rtvIndex++],
                clear,
                NULL,
                0
            );
        }
    }
}

void MRTFrameBuffer::unbind() {

    for (auto buffer : _gBufferTextures) {
        if (buffer.getFormat() == DXGI_FORMAT_D32_FLOAT) {
            buffer.unbindTarget(D3D12_RESOURCE_STATE_DEPTH_WRITE);
        }
        else {
            buffer.unbindTarget(D3D12_RESOURCE_STATE_RENDER_TARGET);
        }
    }
}
