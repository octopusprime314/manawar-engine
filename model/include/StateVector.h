/*
* StateVector is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  StateVector class. Keeps track of kinematic state.
*/

#pragma once
#include "Matrix.h"
#include "Vector4.h"

const float GRAVITY = 9.8f; //meters per second squared 

class StateVector {
    Vector4 _linearPosition;
    Vector4 _angularPosition;
    Vector4 _linearVelocity;
    Vector4 _angularVelocity;
    Vector4 _linearAcceleration;
    Vector4 _angularAcceleration;
    Vector4 _force; //linear force
    Vector4 _torque; //angular force
    float   _mass; //mass of object used to calculate acceleration on objects
public:
    StateVector();
    void update(int milliSeconds);
};