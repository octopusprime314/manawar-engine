/*
* DirectionalShadow is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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

#include "DepthFrameBuffer.h"
#include "Entity.h"
#include "ShadowStaticShader.h"
#include "ShadowAnimatedShader.h"

class Entity;

class DirectionalShadow {

    DepthFrameBuffer      _shadow;
    ShadowStaticShader*   _staticShadowShader;
    ShadowAnimatedShader* _animatedShadowShader;

public:
    DirectionalShadow(GLuint width, GLuint height);
    ~DirectionalShadow();

    GLuint                getTextureContext();
    void                  render(std::vector<Entity*> entityList, Light* light);
};