/*
* Vector4 is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  Vector4 class.  Handles vector transforms.
*/

#pragma once
#include <iostream>

class Vector4 {
    float _vec[4];
public:

    Vector4();
    Vector4(float x, float y, float z, float w);
    float*  getFlatBuffer();
    void    display();
    Vector4 operator / (float scale);
    Vector4 operator + (Vector4 other);
    bool    operator == (Vector4 other);
    bool    operator != (Vector4 other);
    float   getx();
    float   gety();
    float   getz();
    float   getw();
    void    normalize();
    friend std::ostream &operator << (std::ostream& output, Vector4 &other); 
};