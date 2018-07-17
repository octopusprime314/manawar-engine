/*
* ModelBroker is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  The ModelBroker class is a singleton that manages all models in a scene
*/

#pragma once
#include "Model.h"
#include "AnimatedModel.h"
#include <map>
#include <vector>

using ModelMap = std::map<std::string, Model*>;

class ModelBroker {
    ModelBroker();
    ModelMap             _models;
    static ModelBroker*  _broker;
    void                 _gatherModelNames();
    std::string          _strToUpper(std::string s);
public:
    static ModelBroker*  instance();
    ~ModelBroker();
    Model*               getModel(std::string modelName);
    void                 buildModels();
    void                 updateModel(std::string modelName);
    void                 saveModel(std::string modelName);
    void                 addModel(std::string modelName, std::string modelToAdd, Vector4 location);
};