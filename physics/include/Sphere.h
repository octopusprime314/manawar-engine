/*
* Sphere is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  Sphere class. Stores a 3d sphere and it's characteristics
*/

#pragma once
#include "Vector4.h"
class Sphere {

    float   _radius;
    Vector4 _position;
    Vector4 _modelPosition;
public:
    Sphere(float radius, Vector4 position);
    ~Sphere();
    float   getRadius();
    Vector4 getPosition();
    void    offsetPosition(Vector4 position); //Offsets the sphere's current position
};