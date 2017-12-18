/*
* Cube is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  Cube class. 3D Rectangle length, width, height and center position
*/

#pragma once
#include "Vector4.h"

class Cube {
    float   _length; //x dimension
    float   _height; //y dimension
    float   _width;  //z dimension
    Vector4 _center;
public:
    Cube(float length, float width, float height, Vector4 center);
    float   getLength(); //x dimension
    float   getHeight(); //y dimension
    float   getWidth();  //z dimenion
    Vector4 getCenter();
};