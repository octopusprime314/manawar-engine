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
#include "Model.h"
#include "OSP.h"
#include <vector>
#include "GeometryGraphic.h"

class Physics {

    OSP                           _octalSpacePartioner;
    std::vector<Model*>           _models; //Models containing collision Geometry
    std::vector<GeometryGraphic*> _graphics; //Visual objects for debugging physics
    void                          _physicsProcess(int milliseconds); //Physics processing thread
    void                          _slowDetection(); //Keep the slow collision detection around for testing purposes
    DebugShader*                  _debugShader;
    GeometryGraphic*              _octTreeGraphic;
public:
    Physics();
    ~Physics();
    void                          run();
    void                          addModels(std::vector<Model*> models);
    void                          addModel(Model* model);
    void                          visualize();
};