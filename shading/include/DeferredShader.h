/*
* DeferredShader is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  DeferredShader class. Manages all of the g buffer textures used to compute
*  per pixel shading techniques.  Derives off of the basic Shader class and is solely
*  responsible for defining which uniforms should be in the deferred shader model
*  If a uniform does not exist in the shader code there will an error.
*/

#pragma once
#include "Shader.h"
#include "AnimatedModel.h"
#include "ShadowRenderer.h"
#include "MRTFrameBuffer.h"
#include "Light.h"
#include "ViewManager.h"
#include "PointShadowMap.h"
#include "SSAO.h"
#include "EnvironmentMap.h"

class DeferredShader : public Shader {

    GLuint _normalTextureLocation;
    GLuint _diffuseTextureLocation;
    GLuint _positionTextureLocation;
    GLuint _cameraDepthTextureLocation;
    GLuint _mapDepthTextureLocation;
    GLuint _quadBufferContext;
    GLuint _textureBufferContext;
    GLuint _lightLocation;
    GLuint _lightViewLocation;
    GLuint _lightMapViewLocation;
    GLuint _viewsLocation;
    GLuint _pointLightCountLocation;
    GLuint _pointLightPositionsLocation;
    GLuint _pointLightColorsLocation;
    GLuint _pointLightRangesLocation;
    GLuint _pointLightDepthMapLocation;
    GLuint _viewToModelSpaceMatrixLocation;
    GLuint _farPlaneLocation;
    GLuint _ssaoTextureLocation;
    GLuint _environmentMapTextureLocation;
    GLuint _bloomTextureLocation;

    GLuint _skyboxDayTextureLocation;
    GLuint _skyboxNightTextureLocation;
    GLuint _inverseViewLocation;
    GLuint _inverseProjectionLocation;
    Texture* _skyBoxDayTexture;
    Texture* _skyBoxNightTexture;
    GLuint   _vaoContext;

public:
    DeferredShader(std::string shaderName);
    virtual ~DeferredShader();
    void runShader(ShadowRenderer* shadowRenderer, std::vector<Light*>& lights, 
        ViewManager* viewManager, MRTFrameBuffer& mrtFBO, 
        PointShadowMap* pointShadowMap,
        SSAO* ssao,
        EnvironmentMap* environmentMap);
};
