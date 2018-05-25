#include "Blur.h"
#include "SSAO.h"
#include "GLIncludes.h"

Blur::Blur() {
	glGenTextures(1, &_colorBufferBlur);
	glBindTexture(GL_TEXTURE_2D, _colorBufferBlur);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1920, 1080, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Blur::~Blur() {

}

unsigned int Blur::getBlurTexture() {
	return _colorBufferBlur;
}

void Blur::computeBlur(SSAO* ssao) {
    _computeShader.runShader(this, ssao);
}
