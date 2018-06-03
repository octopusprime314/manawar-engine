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
#include "UpdateInterface.h"
#include "Shader.h"
#include "EffectShader.h"
#include "Effect.h"

enum class LightType {
    CAMERA_DIRECTIONAL = 0,
    MAP_DIRECTIONAL,
    POINT,
    SPOTLIGHT
};

class Light : public Effect{
    MVP                     _lightMVP; //Light's Model view matrix for light
    Vector4                 _position; //Position of light
    LightType               _type; //Light type enum
    Vector4                 _color; //Light color
    void                    _updateTime(int time);
    uint64_t                _milliSecondTime;
    bool                    _shadowCaster;

public:
    Light(ViewManagerEvents* eventWrapper, 
        MVP mvp, 
        LightType type, 
        Vector4 color, 
        bool shadowCaster);
    MVP                         getLightMVP();
    MVP                         getCameraMVP();
    Vector4                     getPosition();
    LightType                   getType();
    Vector4&                    getColor();
    float                       getRange();
    bool                        isShadowCaster();
    void                        setMVP(MVP mvp);
    void                        render();

};
