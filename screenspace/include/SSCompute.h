/*
* SSComputeRGB is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  SSCompute class. Generic screen space compute shader that outputs in a specified format
*/

#pragma once
#include "ComputeShader.h"
#include "RenderTexture.h"
#include "ShaderBroker.h"

class SSCompute {
    ComputeShader* _computeShader;
    TextureFormat  _format;
    RenderTexture  _renderTexture;
    static ShaderBroker* _shaderManager;        //Static shader manager for shader reuse purposes, all models have access

public:
    SSCompute(std::string computeShader, GLuint width, GLuint height, TextureFormat format);
    ~SSCompute();
    unsigned int   getTextureContext();
    Texture*       getTexture();
    void           compute(Texture* readTexture);
    void           compute(Texture* readTexture, Texture* writeTexture);
};