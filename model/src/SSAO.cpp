#include "SSAO.h"
#include <random>
#include "GLIncludes.h"
#include "MRTFrameBuffer.h"
#include "ViewManager.h"

SSAO::SSAO() {

	glGenFramebuffers(1, &_ssaoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _ssaoFBO);

	glGenTextures(1, &_ssaoColorBuffer);
	glBindTexture(GL_TEXTURE_2D, _ssaoColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1920, 1080, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ssaoColorBuffer, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

SSAO::~SSAO() {

}

//helper lerp, move to math function utility someday
float lerp(float a, float b, float f) {
	return a + f * (b - a);
}

void SSAO::_generateKernelNoise() {
	// random floats between 0.0 - 1.0
	std::uniform_real_distribution<float> randomFloats(0.0, 1.0); 
	std::default_random_engine generator;

	for (unsigned int i = 0; i < 64; ++i)
	{
		Vector4 sample(
			randomFloats(generator) * 2.0f - 1.0f,
			randomFloats(generator) * 2.0f - 1.0f,
			randomFloats(generator),
			1.0f
		);
		sample.normalize();
		sample = sample * randomFloats(generator);
		float scale = static_cast<float>(i) / 64.0f;

		scale = lerp(0.1f, 1.0f, scale * scale);
		sample = sample * scale;
		_ssaoKernel.push_back(sample);
	}

	for (unsigned int i = 0; i < 16; i++)
	{
		Vector4 noise(
			randomFloats(generator) * 2.0f - 1.0f,
			randomFloats(generator) * 2.0f - 1.0f,
			0.0f,
			1.0f);
		_ssaoNoise.push_back(noise);
	}

	glGenTextures(1, &_noiseTexture);
	glBindTexture(GL_TEXTURE_2D, _noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &_ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void SSAO::computeSSAO(MRTFrameBuffer* mrtBuffer, ViewManager* viewManager) {
	
	glBindFramebuffer(GL_FRAMEBUFFER, _ssaoFBO);

	glClear(GL_COLOR_BUFFER_BIT);

	_ssaoShader.runShader(this, mrtBuffer, viewManager);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

unsigned int SSAO::getNoiseTexture() {
	return _noiseTexture;
}

std::vector<Vector4>& SSAO::getKernel() {
	return _ssaoKernel;
}