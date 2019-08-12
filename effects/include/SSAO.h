/*
* SSAO is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  SSAO class. Screen space ambient occlusion technique using g buffers and compute shader
*  for blurring and smoothing the ssao pass.
*/

#pragma once
#include "Vector4.h"
#include <vector>
#include "SSAOShader.h"
#include "SSCompute.h"
#include "RenderTexture.h"
class MRTFrameBuffer;
class ViewEventDistributor;

class SSAO {

    void                  _generateKernelNoise();
    RenderTexture         _renderTexture;
    unsigned int          _noiseTexture;
    SSAOShader*           _ssaoShader;
    std::vector<Vector4>  _ssaoKernel;
    SSCompute*            _downSample;
    std::vector<Vector4>  _ssaoNoise;
    SSCompute*            _upSample;
    unsigned int          _ssaoFBO;
    AssetTexture*         _noise;
    SSCompute*            _blur;

public:
    SSAO();
    ~SSAO();
    void                  computeSSAO(MRTFrameBuffer*       mrtBuffer,
                                      ViewEventDistributor* viewEventDistributor);
    Texture*              getNoiseTexture();
    Texture*              getSSAOTexture();
    std::vector<Vector4>& getKernel();
    SSCompute*            getBlur();
};