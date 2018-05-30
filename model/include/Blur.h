/*
* Blur is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  Blur class. Compute shaders that blurs a screen space texture
*/

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