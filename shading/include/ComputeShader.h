#pragma once
#include "Shader.h"
class Blur;
class SSAO;

class ComputeShader : public Shader {

	unsigned int _SSAOTextureLocation;
	unsigned int _blurredSSAOTextureLocation;
public:
	ComputeShader();
	~ComputeShader();
	void runShader(Blur* blur, SSAO* ssao);
};