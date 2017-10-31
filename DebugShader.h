#pragma once
#include "Shader.h"

class DebugShader : public Shader{

public:
	DebugShader();
	void build();
	void runShader(Model* model);
};