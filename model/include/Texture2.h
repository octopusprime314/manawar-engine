/*
* Texture2 is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  Texture2 class.  Handles texture coordinate transforms.
*/

#pragma once

class Texture2 {
    float _st[2];
public:

    Texture2();
    Texture2(float s, float t);
    float*  getFlatBuffer();
    void    display();
    Texture2 operator / (float scale);
    Texture2 operator + (Texture2 other);
    float   gets();
    float   gett();
};