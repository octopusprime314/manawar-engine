/*
* DeferredRenderer is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  DeferredRenderer class. Generates G buffers including normal and diffuse buffers.
*  DeferredRenderer is a unique renderer and contains 1 shader with a Multi Render Target
*  buffer design.  All two of the previously mentioned textures are generated using a single frame buffer
*  that include a texture attachment which will be deferred and used for a later draw using lights.
*  All lights will be incorporated in this shader for faster lighting calculations because only pixels
*  visible on the screen will be light shaded.
*/

#pragma once
#include "DeferredShader.h"
#include "DepthFrameBuffer.h"
#include "MRTFrameBuffer.h"
#include "ShadowRenderer.h"
#include "ViewManager.h"
#include "Light.h"
class Model;

class DeferredRenderer {
    
    DeferredShader _deferredShader;
    MRTFrameBuffer _mrtFBO;

public:
    DeferredRenderer();
    ~DeferredRenderer();
    void deferredLighting(ShadowRenderer* shadowRenderer, 
        std::vector<Light*>& lights, ViewManager* viewManager);
    void bind();
    void unbind();
};