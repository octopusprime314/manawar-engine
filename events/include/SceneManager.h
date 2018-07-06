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
#include "Physics.h"
#include <vector>

class ViewManager;
class DeferredRenderer;
class ShadowRenderer;
class AudioManager;
class PointShadowMap;
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

class SceneManager {
    ViewManager*         _viewManager; //manages the view/camera matrix from the user's perspective
    std::vector<Model*>  _modelList; //Contains models active in scene
    std::vector<Light*>  _lightList; //Contains all lights in a scene
    Physics*             _physics; //Manages physical interactions between models
    DeferredRenderer*    _deferredRenderer; //Manages deferred shading g buffers
    ForwardRenderer*     _forwardRenderer; //Manages forward shading transparent objects
    ShadowRenderer*      _shadowRenderer;   //Manages shadow rendering
    PointShadowMap*      _pointShadowMap; //Manages point shadow lights
    AudioManager*        _audioManager;  // Manages audio playback
    SSAO*                _ssaoPass;
    EnvironmentMap*      _environmentMap;
    Water*               _water;
    DeferredFrameBuffer* _deferredFBO;
    Bloom*               _bloom;
    MergeShader*         _mergeShader;
    SSCompute*           _add;
    Terminal*            _terminal;

    void _preDraw(); //Prior to drawing objects call this function
    void _postDraw(); //Post of drawing objects call this function
public:
    SceneManager(int* argc, char** argv, unsigned int viewportWidth, unsigned int viewportHeight,
        float nearPlaneDistance, float farPlaneDistance);
    ~SceneManager();
};