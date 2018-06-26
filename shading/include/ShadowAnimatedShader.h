/*
* ShadowAnimatedShader is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  ShadowAnimatedShader class. Derives off of the Shadow Static Shader class and is solely
*  responsible for defining additional uniforms should be in the shadow animated shader model
*  If a uniform does not exist in the shader code there will be an error.
*/

#pragma once
#include "ShadowStaticShader.h"
#include "AnimatedModel.h"

class ShadowAnimatedShader : public ShadowStaticShader {

public:
    ShadowAnimatedShader(std::string shaderName);
    virtual     ~ShadowAnimatedShader();
    void        runShader(Model* model, Light* light);
};
