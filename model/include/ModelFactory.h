/*
* ModelFactory is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  ModelFactory class. Manages various model type creation
*/

#pragma once
#include "AnimatedModel.h"
#include "Model.h"

class ViewManager;
//ModelFactory is a singleton factory
class ModelFactory {
    //Make constructor private so it can't be instantiated
    ModelFactory();
    static ModelFactory* _factory;
    ViewManagerEvents*   _viewEventWrapper;
public:
 
    ~ModelFactory();
    static ModelFactory* instance();
    Model*               makeModel(std::string name);
    AnimatedModel*       makeAnimatedModel(std::string name);
    void                 setViewWrapper(ViewManager* viewManager); //Sets the reference to the view model's event object
};
