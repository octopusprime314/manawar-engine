/*
* ForwardRenderer is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  The ForwardRenderer class renders all of the non-opaque (transparent) objects
*/

#pragma once
#include "MRTFrameBuffer.h"
#include "InstancedForwardShader.h"

class ForwardRenderer {
    ForwardShader*          _forwardShader;
    InstancedForwardShader* _instancedForwardShader;
    static ShaderBroker*    _shaderManager;        //Static shader manager for shader reuse purposes, all models have access

public:
    ForwardRenderer();
    ~ForwardRenderer();
    void forwardLighting(std::vector<Model*>& modelList, ViewManager* viewManager, ShadowRenderer* shadowRenderer,
        std::vector<Light*>& lights, PointShadowMap* pointShadowMap);
};