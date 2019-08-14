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
#include "ViewEvents.h"


using ModelMap = std::map<std::string, Model*>;

class ModelBroker {

    ModelBroker();
    std::string                  _strToUpper(std::string s);
    std::string                  _strToLower(std::string s);
    void                         _gatherModelNames();
    
    static ViewEventDistributor* _viewManager;
    std::vector<std::string>     _modelNames;
    ModelMap                     _models;
    static ModelBroker*          _broker;

public:
    ~ModelBroker();

    static void                  setViewManager(ViewEventDistributor*    viewEventDistributor);
    void                         clearChanges(  std::string              modelName);
    void                         addTileModel(  std::string              modelName,
                                                std::string              modelToAdd,
                                                Vector4                  location,
                                                std::vector<std::string> textures);
    void                         removeModel(   Entity*                  entityToRemove,
                                                std::string              modelRemovedFrom);
    void                         updateModel(   std::string              modelName);
    void                         saveModel(     std::string              modelName);
    void                         addModel(      std::string              modelName,
                                                std::string              modelToAdd,
                                                Vector4                  location,
                                                Vector4                  rotation);
    Model*                       getModel(      std::string              modelName);
    Model*                       getModel(      std::string              modelName,
                                                Vector4                  pos);
    static ViewEventDistributor* getViewManager();
    std::vector<std::string>     getModelNames();
    void                         buildModels();
    static ModelBroker*          instance();
};