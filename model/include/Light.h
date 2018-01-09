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
*  Light class. Describes a model that emits light and subscribes to view changes
*/

#pragma once
#include "ViewManagerEvents.h"
#include "MVP.h"
class Light : public ViewManagerEvents {
    MVP                         _mvp; //Model view matrix container
    Vector4                     _lightDir; //Directional light vector
    static ViewManagerEvents*   _viewEventWrapper;

protected:
    void                        _updateView(Matrix view); //Get view matrix updates

public:
    Light(Vector4 lightDir);
    Vector4                     getLightDirection();
    float*                      getNormalBuffer();
    Matrix                      getNormalMatrix();
    static void                 setViewWrapper(ViewManagerEvents* wrapper);
};
