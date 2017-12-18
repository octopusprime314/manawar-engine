/*
* SceneManager is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  SceneManager class. Contains the view manager and models stored in the view space
*/

#pragma once
#include "ViewManager.h"
#include "ModelFactory.h"
#include <vector>
#include "Physics.h"

class SceneManager {
    ViewManager*        _viewManager; //manages the view/camera matrix from the user's perspective
    std::vector<Model*> _modelList; //Contains models active in scene
    ModelFactory*       _modelFactory; //Creates new models
    Physics             _physics; //Manages physical interactions between models
public:
    SceneManager(int* argc, char** argv, unsigned int viewportWidth, unsigned int viewportHeight, 
        float nearPlaneDistance, float farPlaneDistance);
    ~SceneManager();
};