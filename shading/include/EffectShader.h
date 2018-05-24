/*
* EffectShader is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  EffectShader class. Generic effects class 
*/

#pragma once
#include "Shader.h"
#include "Matrix.h"
#include "MVP.h"
class Light;
class ViewManager;

class EffectShader : public Shader {

protected:
    GLint       _timeLocation;
    GLint       _lightPosLocation;
    GLint       _modelViewProjectionLocation;
    GLint       _fireTypeLocation;
    GLint       _farPlaneLocation;
    GLuint       _vaoContext;
public:
    EffectShader(std::string shaderName);
    virtual ~EffectShader();
    virtual void runShader(Light* light, MVP& cameraMVP, float seconds);
};
