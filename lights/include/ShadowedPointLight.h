/*
* ShadowedPointLight is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  ShadowedPointLight class. 
*  Describes a model that emits light with a depth shadow fbo
*/

#pragma once
#include "Light.h"
#include "PointShadow.h"
#include "Entity.h"

class ShadowedPointLight : public Light {

    PointShadow              _shadow;
public:
    ShadowedPointLight(ViewEvents* eventWrapper,
        MVP mvp,
        EffectType effect,
        Vector4 color);
    GLuint                      getDepthTexture();
    virtual void                render();
    virtual void                renderShadow(std::vector<Entity*> entityList);
    virtual void                renderDebug();

};
