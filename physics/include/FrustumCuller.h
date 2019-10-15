/*
* FrustumCuller is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  FrustumCuller class. Trims down the number of vertices needed to be passed through the
* graphics VS pipeline by using AABBs from an Octal Space Partioning structure.  The AABBs 
* that are within the view/eye frustum will provide all of the vertexes that need to be rendered.
*/

#pragma once
#include "Vector4.h"
#include "OSP.h"
#include <vector>
#include "GeometryGraphic.h"
#include "Light.h"

class Entity;

class FrustumCuller {
    OSP*              _octalSpacePartitioner;
    GeometryGraphic*  _octTreeGraphic;
    DebugShader*      _debugShader;
    std::vector<Cube> _singleAABB;
public:
    FrustumCuller(                   Entity* entity,
                                     float   aabbDimension,
                                     int     aabbMaxTriangles);
    FrustumCuller(                   Entity* entity,
                                     Cube    aabbCube);
    bool              getVisibleVAO( Entity* entity);
    static bool       getVisibleAABB(Entity*               entity,
                                     std::vector<Vector4>& frustumPlanes);
    static bool       getVisibleOBB( Entity* entity,
                                     Matrix inverseViewProjection, 
                                     Light* light);
    std::vector<int>  getVisibleVAOs();
    void              visualize();
    OSP*              getOSP();
};