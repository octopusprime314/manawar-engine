/*
* EngineManager is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  EngineManager class. Contains the view manager and models stored in the view space
*/

#pragma once
#include "Physics.h"
#include <vector>

class ViewEventDistributor;
class DeferredRenderer;
class ShadowRenderer;
class AudioManager;
class PointShadow;
class ForwardRenderer;
class SSAO;
class EnvironmentMap;
class Water;
class FontRenderer;
class DeferredFrameBuffer;
class MergeShader;
class Bloom;
class SSCompute;
class ShaderBroker;
class Terminal;
class Entity;
class IOEventDistributor;

class EngineManager {
    ViewEventDistributor*        _viewManager; //manages the view/camera matrix from the user's perspective
    static std::vector<Entity*>  _entityList; //Contains models active in scene
    std::vector<Light*>          _lightList; //Contains all lights in a scene
    Physics*                     _physics; //Manages physical interactions between models
    DeferredRenderer*            _deferredRenderer; //Manages deferred shading g buffers
    ForwardRenderer*             _forwardRenderer; //Manages forward shading transparent objects
    AudioManager*                _audioManager;  // Manages audio playback
    SSAO*                        _ssaoPass;
    EnvironmentMap*              _environmentMap;
    Water*                       _water;
    DeferredFrameBuffer*         _deferredFBO;
    Bloom*                       _bloom;
    MergeShader*                 _mergeShader;
    SSCompute*                   _add;
    Terminal*                    _terminal;
    IOEventDistributor*          _glfwContext;
                                 
    void                         _preDraw(); //Prior to drawing objects call this function
    void                         _postDraw(); //Post of drawing objects call this function
public:
    EngineManager(int* argc, char** argv);
    ~EngineManager();
    static Entity*               addEntity(Model* model, Matrix transform);
    static std::vector<Entity*>* getEntityList();
};