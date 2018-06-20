#include "SSComputeRGB.h"
#include "GLIncludes.h"

SSComputeRGB::SSComputeRGB(std::string computeShader) {

    _computeShader = new ComputeShader(computeShader);

    glGenTextures(1, &_colorBuffer);
    glBindTexture(GL_TEXTURE_2D, _colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenPixelWidth, screenPixelHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
}

SSComputeRGB::~SSComputeRGB() {

}

unsigned int SSComputeRGB::getTextureContext() {
    return _colorBuffer;
}

void SSComputeRGB::compute(GLuint readTexture) {
    _computeShader->runShader(_colorBuffer, readTexture, true);
}