#include "MergeShader.h"
#include "GLIncludes.h"
#include "MRTFrameBuffer.h"
#include "MVP.h"
#include "GLSLShader.h"
#include "HLSLShader.h"
#include "EngineManager.h"
#include "DXLayer.h"

MergeShader::MergeShader() {
    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        _shader = new GLSLShader("mergeShader");
        glGenVertexArrays(1, &_dummyVAO);
    }
    else {
        std::vector<DXGI_FORMAT>* formats = new std::vector<DXGI_FORMAT>();
        formats->push_back(DXGI_FORMAT_R8G8B8A8_UNORM);
        formats->push_back(DXGI_FORMAT_D32_FLOAT);
        _shader = new HLSLShader("mergeShader", "", formats);
    }
}

MergeShader::~MergeShader() {

}

void MergeShader::runShader(Texture* deferredTexture, Texture* velocityTexture) {

    //LOAD IN SHADER
    _shader->bind(); //use context for loaded shader
    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {

        glBindVertexArray(_dummyVAO);
    }
    else {
        _shader->bindAttributes(nullptr);
    }

    if (deferredTexture != nullptr) {
        _shader->updateData("deferredTexture", GL_TEXTURE0, deferredTexture);
    }
    if (velocityTexture != nullptr) {
        _shader->updateData("velocityTexture", GL_TEXTURE1, velocityTexture);
    }

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)4);
    }
    else {
        _shader->draw(0, 1, 3);
    }

    _shader->unbindAttributes();

    _shader->unbind();
}
