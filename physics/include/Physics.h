/*
* Physics is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  Physics class. Manages all of the models collision properties
*/

#pragma once
#include "Entity.h"
#include "OSP.h"
#include <vector>
#include "GeometryGraphic.h"

using ModelIntersections = std::map<Entity*, std::set<Triangle*>>;
class Physics {

    //Physics processing thread
    void                          _physicsProcess(int milliseconds);
    //Keep the slow collision detection around for testing purposes
    void                          _slowDetection();

    ModelIntersections            _triangleIntersectionList;
    OSP                           _octalSpacePartioner;
    GeometryGraphic*              _octTreeGraphic;
    DebugShader*                  _debugShader;
    //Entities containing collision Geometry
    std::vector<Entity*>          _entities;
    //Visual objects for debugging physics
    std::vector<GeometryGraphic*> _graphics;
    std::mutex                    _lock;
public:
    Physics();
    ~Physics();
    void                          addEntities(std::vector<Entity*> entities);
    void                          addEntity(Entity* entity);
    void                          visualize();
    void                          run();
};