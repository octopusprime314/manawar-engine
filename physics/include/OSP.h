/*
* OSP is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  OSP class. Octary Space Partition class that subdivides primitive geometries for
*  collision handling, etc.  Recursively creates subspaces based upon the maximum
*  number of collision primitices specified by _maxGeometries.  The other tunable
*  parameter is the size of the 3D space being capturwwwwwwwwwwwwwwwwwed by the OSP.
*/
#pragma once
#include <vector>
#include "OctTree.h"
#include "Cube.h"
#include "Geometry.h"
#include <map>

class OSP {

    bool                               _insertSphereSubspaces(Entity* entity,
                                                              Sphere& sphere,
                                                              OctNode<Cube*>* node);
    void                               _buildOctetTree(       Cube* rectangle,
                                                              OctNode<Cube*>* node);
    void                               _getChildren(          std::vector<Cube>* cubes,
                                                              OctNode<Cube*>* node);

    //Caches previous list of subspace cubes for early out testing
    std::map<Sphere*, std::set<Cube*>> _sphereCubeCache;
    //Describes the cubic 3D space dimensions of the OSP volume
    float                              _cubicDimension;
    //The largest amount of geometry items in a subspace of _dimension^3
    int                                _maxGeometries;
    //AABBs that are visible in the frustum
    std::vector<OctNode<Cube*>*>       _frustumLeaves;
    //End nodes that are used for collision testing
    std::vector<OctNode<Cube*>*>       _ospLeaves;
    //The smallest 3D cubic space a tree
    int                                _subSpace;
    //Make a Cube Octary tree
    OctTree<Cube*>                     _octTree;

public:
    OSP(float cubicDimension,
        int   maxGeometries);
    ~OSP();
    void                               generateGeometryOSP(std::vector<Entity*>& entities);
    void                               generateRenderOSP(  Entity*               entity);
    void                               updateOSP(          std::vector<Entity*>& entities);
    std::vector<int>                   getVisibleFrustumCulling();
    std::vector<OctNode<Cube*>*>*      getFrustumLeaves();
    std::vector<Cube>*                 getFrustumCubes();
    std::vector<OctNode<Cube*>*>*      getOSPLeaves();
    std::vector<Cube>*                 getCubes();
 
};