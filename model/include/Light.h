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

enum class LightType{
        CAMERA_DIRECTIONAL = 0,
        MAP_DIRECTIONAL,
        POINT,
        SPOTLIGHT
    };

class Light : UpdateInterface{
    MVP                     _mvp; //Model view matrix container
    Matrix                  _view; //tracks camera view matrix
    Matrix                  _projection; //tracks camera projection matrix
    Vector4                 _position; //Position of light
    LightType               _type; //Light type enum
    Vector4                 _color; //Light color
    float                   _range; //Light range for point lights, etc

public:
    Light(ViewManagerEvents* eventWrapper, 
        MVP mvp, 
        LightType type, 
        Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f }, 
        float range = 0.0f);
    MVP                         getMVP();
    Vector4                     getPosition();
    LightType                   getType();
    Vector4&                    getColor();
    float                       getRange();

protected:
    void                        _updateKeyboard(int key, int x, int y); //Do stuff based on keyboard upate
    void                        _updateReleaseKeyboard(int key, int x, int y);
    void                        _updateMouse(double x, double y); //Do stuff based on mouse update
    void                        _updateDraw(); //Do draw stuff
    void                        _updateView(Matrix view); //Get view matrix updates
    void                        _updateProjection(Matrix projection); //Get projection matrix updates
};
