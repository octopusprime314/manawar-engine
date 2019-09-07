#include "BlitDepthShader.h"
#include "GLIncludes.h"
#include "GLSLShader.h"
#include "HLSLShader.h"
#include "EngineManager.h"

BlitDepthShader::BlitDepthShader() {
    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        _shader = new GLSLShader("blitDepthShader");
        glGenVertexArrays(1, &_dummyVAO);
    }
    else {
        std::vector<DXGI_FORMAT>* formats = new std::vector<DXGI_FORMAT>();
        formats->push_back(DXGI_FORMAT_R32_TYPELESS);
        _shader = new HLSLShader("blitDepthShader", "", formats);
    }
}

BlitDepthShader::~BlitDepthShader() {

}

void BlitDepthShader::runShader(RenderTexture* inDepthBuffer,
                                RenderTexture* outDepthBuffer) {

    _shader->bind();
    if(EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        glBindVertexArray(_dummyVAO);
    }
    else {
        _shader->bindAttributes(nullptr);
    }

    _shader->updateData("inDepthTexture", GL_TEXTURE1, inDepthBuffer);

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)4);
    }
    else {
        _shader->draw(0, 1, 3);
    }
    _shader->unbindAttributes();
    _shader->unbind();
}
