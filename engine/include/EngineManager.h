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
class DXLayer;
class RayTracingPipelineShader;

enum class GraphicsLayer {
    OPENGL,
    DX12,
    DXR,
    DXR_EXPERIMENTAL,
};

class EngineManager {

    static RayTracingPipelineShader* _rayTracingPipeline;
    static std::mutex                _entityListLock;
    static Entity*                   _shadowEntity;

    //Manages deferred shading g buffers
    DeferredRenderer*            _deferredRenderer;
    //Manages forward shading transparent objects
    ForwardRenderer*             _forwardRenderer;
    EnvironmentMap*              _environmentMap;
    static GraphicsLayer         _graphicsLayer;
    bool                         _useRaytracing;
    bool                         _generatorMode;
    // Manages audio playback
    AudioManager*                _audioManager;
    //manages the view/camera matrix from the user's perspective
    ViewEventDistributor*        _viewManager;
    DeferredFrameBuffer*         _deferredFBO;
    MergeShader*                 _mergeShader;
    unsigned int                 _pathCounter;
    //Contains models active in scene
    static std::vector<Entity*>  _entityList;
    IOEventDistributor*          _inputLayer;
    //Contains all lights in a scene
    std::vector<Light*>          _lightList;
    Terminal*                    _terminal;
    SSAO*                        _ssaoPass;
    //Manages physical interactions between models
    Physics*                     _physics;
    Water*                       _water;
    Bloom*                       _bloom;
    SSCompute*                   _add;
    unsigned int                 _frameCounter = 0;

    //Post of drawing objects call this function
    void                         _postDraw();
    //Prior to drawing objects call this function
    void                         _preDraw();
public:
    EngineManager(int*      argc,
                  char**    argv,
                  HINSTANCE hInstance,
                  int       nCmdShow);

    ~EngineManager();
    static Entity*                   addEntity(Model* model,
                                               Matrix transform,
                                               bool temporaryModel);

    static void                      removeEntity(Entity* entity);
    static std::vector<Entity*>*     getEntityList();
    static GraphicsLayer             getGraphicsLayer();
    static RayTracingPipelineShader* getRTPipeline();

};