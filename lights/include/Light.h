/*
* Light is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  Light class. Describes a model that emits light
*/

#pragma once
#include "MVP.h"
#include "EventSubscriber.h"
#include "Shader.h"
#include "EffectShader.h"
#include "Effect.h"
#include "VAO.h"
#include "DebugShader.h"
#include <vector>

enum class LightType {
    DIRECTIONAL = 0,
    SHADOWED_DIRECTIONAL,
    POINT,
    SHADOWED_POINT,
    SPOTLIGHT,
    SHADOWED_SPOTLIGHT
};

//The amount of milliseconds in 24 hours
const uint64_t dayLengthMilliseconds = 24 * 60 * 60 * 1000;

class Light : public Effect {

protected:

    uint64_t     _milliSecondTime;
    DebugShader* _debugShader;
    MVP          _lightMVP;
    Vector4      _position;
    Vector4      _color;
    LightType    _type;
    VAO          _vao;

    void         _updateTime(int time);

public:
    Light(ViewEvents* eventWrapper,
          MVP         mvp,
          LightType   type,
          EffectType  effect,
          Vector4     color);

    virtual void renderShadow(std::vector<Entity*> entityList);
    Vector4      getLightDirection();
    bool         isShadowCaster();
    void         setMVP(MVP mvp);
    MVP          getCameraMVP();
    MVP          getLightMVP();
    Vector4      getPosition();
    virtual void renderDebug() {};
    float        getHeight();
    Vector4&     getColor();
    float        getRange();
    float        getWidth();
    LightType    getType();
    virtual void render();
};
