/*
* PointShadowRenderer is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  PointShadowRenderer class. Uses a cube map texture class from point lights in the scene
*  to generate shadows.
*/

#pragma once

#include "ShadowAnimatedPointShader.h"
#include "CubeMapRenderer.h"
class Entity;

class PointShadowMap : public CubeMapRenderer {

    ShadowPointShader*          _pointShadowShader; //Shader that generates point light cube map shadows
    ShadowAnimatedPointShader*  _pointAnimatedShadowShader; //Animated Shader that generates point light cube map shadows
    static ShaderBroker*        _shaderManager;        //Static shader manager for shader reuse purposes, all models have access

public:
    PointShadowMap(GLuint width, GLuint height);
    ~PointShadowMap();

    void                       render(std::vector<Entity*> entityList, Light* light);
};