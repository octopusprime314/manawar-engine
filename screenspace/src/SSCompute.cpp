#include "SSCompute.h"
#include "GLIncludes.h"

SSCompute::SSCompute(std::string computeShader, Format format) {

    _computeShader = new ComputeShader(computeShader);
    _format = format;

    glGenTextures(1, &_colorBuffer);
    glBindTexture(GL_TEXTURE_2D, _colorBuffer);
    if (_format == Format::RGBUB) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenPixelWidth, screenPixelHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
    else if (_format == Format::RF) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenPixelWidth, screenPixelHeight, 0, GL_RGB, GL_FLOAT, NULL);
    }
    else if (_format == Format::RGBF) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenPixelWidth, screenPixelHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    }
    else if (_format == Format::RU) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenPixelWidth, screenPixelHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
}

SSCompute::~SSCompute() {

}

unsigned int SSCompute::getTextureContext() {
    return _colorBuffer;
}

void SSCompute::compute(GLuint readTexture) {
    _computeShader->runShader(_colorBuffer, readTexture, _format);
}