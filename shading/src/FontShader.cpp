#include "FontShader.h"
#include "Font.h"
#include "TextureBroker.h"

FontShader::FontShader(std::string shaderName) : Shader(shaderName)
{
    _textureLocation = glGetUniformLocation(_shaderContext, "tex");
}

FontShader::~FontShader()
{
}

void FontShader::runShader(FontRenderer& fnt, std::string& s)
{
    TextureBroker* pTb = TextureBroker::instance();
    Texture* tex = pTb->getTexture("../assets/textures/font/ubuntu_mono_regular_0.tga");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(_shaderContext); //use context for loaded shader
    glCheck();
    glUniform1i(_textureLocation, 0);
    glCheck();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex->getContext());
    glCheck();

    glBindVertexArray(fnt.getVao());
    glDrawArrays(GL_TRIANGLES, 0, 6 * static_cast<GLsizei>(s.size()));
    glCheck();

    glDisable(GL_BLEND);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture
    glUseProgram(0);//end using this shader
}
