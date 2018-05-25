#pragma once
#include "ComputeShader.h"
class SSAO;
class Blur {

    ComputeShader _computeShader;
	unsigned int _colorBufferBlur;
public:
	Blur();
	~Blur();
	unsigned int getBlurTexture();
    void computeBlur(SSAO* ssao);
};