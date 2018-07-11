/*
* ShadowRenderer is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  ShadowRenderer class. Generates depth texture for shadow processing
*/

#pragma once
#include "ShadowAnimatedShader.h"
#include "Light.h"
#include "Model.h"
#include "AnimatedModel.h"
#include <vector>
#include "DepthFrameBuffer.h"
#include "ShaderBroker.h"
class Model;
class ViewManager;
class Entity;

class ShadowRenderer {

    ShadowStaticShader*   _staticShadowShader;   //Shader that generates static geometry shadows
    ShadowAnimatedShader* _animatedShadowShader; //Shader that generates animated geometry shadows
    DepthFrameBuffer      _staticShadowFBO;      //Depth frame buffer object for static objects that do not change
    DepthFrameBuffer      _animatedShadowFBO;    //Depth frame buffer object for animated objects that change
    DepthFrameBuffer      _mapShadowFBO;         //Low resolution shadow mapping for the entire map
    bool                  _staticRendered;       //boolean to only get static object shadows at startup
   
public:
    ShadowRenderer(Light* sunLightCam, Light* sunLightMap);
    ~ShadowRenderer();
    void                  generateShadowBuffer(std::vector<Entity*> entityList, std::vector<Light*>& lights);
    GLuint                getStaticDepthTexture();
    GLuint                getAnimatedDepthTexture();
    GLuint                getMapDepthTexture();
};
