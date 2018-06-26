/*
* ForwardRenderer is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  The ForwardShader class shades all of the transparent objects onto the existing deferred shading buffer
*  by first doing an alpha test to discard the fragment and then a depth test with the g pass depth buffer
*  to draw or discard the fragment
*/

#pragma once
#include "Shader.h"
#include <vector>
class ViewManager;
class ShadowRenderer;
class PointShadowMap;

class ForwardShader : public Shader {

public:
    ForwardShader(std::string vertexShaderName, std::string fragmentShaderName = "");
    ~ForwardShader();
    void runShader(Model* model, ViewManager* viewManager, ShadowRenderer* shadowRenderer,
        std::vector<Light*>& lights, PointShadowMap* pointShadowMap);
};
