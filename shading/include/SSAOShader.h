/*
* SSAOShader is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
* Copyright (c) 2018 Peter Morley.
*
* ReBoot is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, version 3.
*
* ReBoot is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/**
*  SSAOShader class. Takes in position and normal g buffers to generate a ssao frame buffer.
*/
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
    unsigned int _dummyVAO;
public:
	SSAOShader();
	~SSAOShader();
	void runShader(SSAO* ssao, MRTFrameBuffer* mrtBuffer, ViewManager* viewManager);
};