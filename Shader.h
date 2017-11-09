/*
* Shader is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  Shader class. Basic shader class that stores uniform location and operates
*  on model objects when rendering is done.  Contains a vertex and fragment
*  shader implementation.
*/

#pragma once
#include <string>
#include <iostream>
#include "GLIncludes.h"

class Model;

//Simple shader loading class that should be derived from to create more complex shaders
class Shader {
protected:
    GLuint      _shaderContext; //keeps track of the shader context
    GLint       _viewLocation;
    GLint       _modelLocation;
    GLint       _projectionLocation;
    GLint       _normalLocation;
    GLhandleARB _compile(char* filename, unsigned int type);
    void        _link(GLhandleARB vertexShaderHandle, GLhandleARB fragmentShaderHandle);
public:
    Shader();
    virtual ~Shader();
    virtual void build();
    virtual void runShader(Model* model);
    GLuint       getShaderContext();
    GLint        getViewLocation();
    GLint        getModelLocation();
    GLint        getProjectionLocation();
    GLint        getNormalLocation();
};
