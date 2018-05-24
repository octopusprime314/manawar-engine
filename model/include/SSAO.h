#pragma once
#include "Vector4.h"
#include <vector>
#include "SSAOShader.h"
class MRTFrameBuffer;
class ViewManager;

class SSAO {

	void				 _generateKernelNoise();
	std::vector<Vector4> _ssaoKernel;
	std::vector<Vector4> _ssaoNoise;
	unsigned int		 _noiseTexture;
	unsigned int		 _ssaoFBO;
	unsigned int		 _ssaoColorBuffer;
    SSAOShader		     _ssaoShader;
public:
	SSAO();
	~SSAO();
	void computeSSAO(MRTFrameBuffer* mrtBuffer, ViewManager* viewManager);
	unsigned int getNoiseTexture();
	std::vector<Vector4>& getKernel();
};