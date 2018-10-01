#include "MergeShader.h"
#include "GLIncludes.h"
#include "MRTFrameBuffer.h"
#include "MVP.h"
#include "GLSLShader.h"
#include "HLSLShader.h"
#include "EngineManager.h"

MergeShader::MergeShader() {
    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        _shader = new GLSLShader("mergeShader");
    }
    else {
        _shader = new HLSLShader("mergeShader");
    }
    glGenVertexArrays(1, &_dummyVAO);
}

MergeShader::~MergeShader() {

}

void MergeShader::runShader(Texture* deferredTexture, Texture* velocityTexture) {

    //LOAD IN SHADER
    _shader->bind(); //use context for loaded shader
    glBindVertexArray(_dummyVAO);

    _shader->updateData("deferredTexture", GL_TEXTURE0, deferredTexture);
    _shader->updateData("velocityTexture", GL_TEXTURE1, velocityTexture);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)4);

    glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture
    glBindVertexArray(0);
    glUseProgram(0);//end using this shader
}
