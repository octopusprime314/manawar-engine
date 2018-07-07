/*
* Geometry is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  Geometry class. Stores a collection of 3D geometry primitives which
*  can be both a collection of triangles and spheres
*/

#pragma once
#include "Sphere.h"
#include "Triangle.h"
#include <vector>

enum class GeometryType {
    Triangle = 0,
    Sphere = 1
};

class Geometry {

    std::vector<Sphere>    _spheres;
    std::vector<Triangle>  _triangles;

public:
    Geometry();
    ~Geometry();
    void                   addTriangle(Triangle triangle);
    void                   addSphere(Sphere sphere);
    std::vector<Triangle>* getTriangles();
    std::vector<Sphere>*   getSpheres();
    void                   updatePosition(Vector4 position);
};