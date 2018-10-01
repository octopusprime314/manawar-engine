#include "FontShader.h"
#include "Font.h"
#include "TextureBroker.h"
#include "GLSLShader.h"
#include "HLSLShader.h"
#include "EngineManager.h"

FontShader::FontShader(std::string shaderName) {
    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        _shader = new GLSLShader(shaderName);
    }
    else {
        _shader = new HLSLShader(shaderName);
    }
}

FontShader::~FontShader() {
}

void FontShader::runShader(GLuint vao, std::string& s) {
    TextureBroker* pTb = TextureBroker::instance();
    Texture* tex = pTb->getTexture("../assets/textures/font/ubuntu_mono_regular_0.png");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    _shader->bind(); //use context for loaded shader

    _shader->updateData("tex", GL_TEXTURE0, tex);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6 * static_cast<GLsizei>(s.size()));
    glCheck();

    glDisable(GL_BLEND);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture
    glUseProgram(0);//end using this shader
}
