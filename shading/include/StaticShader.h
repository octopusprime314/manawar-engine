/*
* StaticShader is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  StaticShader class. Derives off of the basic Shader class and is solely
*  responsible for defining which uniforms should be in the static shader model
*  If a uniform does not exist in the shader code there will an error.
*/

#pragma once
#include "RenderShader.h"

class StaticShader : public RenderShader{

protected:
    GLint       _viewLocation;
    GLint       _modelLocation;
    GLint       _projectionLocation;
    GLint       _normalLocation;
    GLint       _textureLocation;
public:
    StaticShader(std::string shaderName);
    virtual ~StaticShader();
    virtual void runShader(Model* model);
    GLint        getViewLocation();
    GLint        getModelLocation();
    GLint        getProjectionLocation();
    GLint        getNormalLocation();
    GLint        getTextureLocation();
};
