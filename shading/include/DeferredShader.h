/*
* DeferredShader is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
* Copyright (c) 2017 Peter Morley.
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
*  DeferredShader class. Manages all of the g buffer textures used to compute
*  per pixel shading techniques.  Derives off of the basic Shader class and is solely
*  responsible for defining which uniforms should be in the deferred shader model
*  If a uniform does not exist in the shader code there will an error.
*/

#pragma once
#include "Shader.h"
#include "AnimatedModel.h"
#include "MRTFrameBuffer.h"
#include "Light.h"
#include "ViewEventDistributor.h"
#include "PointShadow.h"
#include "SSAO.h"
#include "EnvironmentMap.h"

class DeferredShader : public ShaderBase {

    AssetTexture* _skyBoxDayTexture;
    AssetTexture* _skyBoxNightTexture;
    GLuint   _vaoContext;

public:
    DeferredShader(std::string shaderName);
    virtual ~DeferredShader();
    void runShader(std::vector<Light*>& lights,
                   ViewEventDistributor* viewEventDistributor, 
                   MRTFrameBuffer& mrtFBO,
                   SSAO* ssao,
                   EnvironmentMap* environmentMap);
};
