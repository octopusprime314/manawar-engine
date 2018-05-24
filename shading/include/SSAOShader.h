#pragma once
#include "ViewManager.h"
#include "Shader.h"
class SSAO;
class MRTFrameBuffer;

class SSAOShader : public Shader{

	unsigned int _positionTextureLocation;
	unsigned int _normalTextureLocation;
	unsigned int _noiseTextureLocation;
	unsigned int _kernelLocation;
	unsigned int _projectionLocation;
public:
	SSAOShader();
	~SSAOShader();
	void runShader(SSAO* ssao, MRTFrameBuffer* mrtBuffer, ViewManager* viewManager);
};