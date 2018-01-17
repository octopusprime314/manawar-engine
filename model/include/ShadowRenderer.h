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
#include "Shader.h"
#include "ShadowFrameBuffer.h"
#include "Light.h"
#include "Model.h"
#include "AnimatedModel.h"
#include <vector>
class Model;

class ShadowRenderer {

    Shader            _staticShadowShader;
    Shader            _animatedShadowShader;
    GLuint            _bonesLocation;
    ShadowFrameBuffer _shadowFBO;
    Matrix            _lightShadowView;
    Vector4           _lightPosition;
    Matrix            _orthographicProjection;

public:
    ShadowRenderer();
    ~ShadowRenderer();
    void generateShadowBuffer(std::vector<Model*> modelList);
    GLuint getDepthTexture();
    Matrix getLightShadowView();
    Matrix getLightShadowProjection();
    Vector4 getLightPosition();
};
