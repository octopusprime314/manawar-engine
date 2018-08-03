/*
* ShadowedDirectionalLight is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  ShadowedDirectionalLight class. 
*  Describes a model that emits light with a depth shadow fbo
*/

#pragma once
#include "Light.h"
#include "DepthFrameBuffer.h"
#include "Entity.h"
#include "DirectionalShadow.h"
#include "ShaderBroker.h"

class ShadowedDirectionalLight : public Light {

    DirectionalShadow     _shadow;
public:
    ShadowedDirectionalLight(ViewManagerEvents* eventWrapper,
        MVP mvp,
        EffectType effect,
        Vector4 color);

    GLuint                getDepthTexture();
    virtual void          render();
    virtual void          renderShadow(std::vector<Entity*> entityList);
    virtual void          renderDebug();
};
