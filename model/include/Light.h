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

enum class LightType{
        DIRECTIONAL = 0,
        POINT,
        SPOTLIGHT
    };

class Light{
    MVP                     _mvp; //Model view matrix container
    Vector4                 _position; //Position of light
    LightType               _type; //Light type enum

public:
    Light(MVP mvp, LightType type);
    MVP&                        getMVP();
    Vector4&                    getPosition();
    LightType                   getType();
};
