/*
* ShaderBroker is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  The ShaderBroker class is a singleton that manages all shaders in a scene
*/

#pragma once
#include "Shader.h"
#include <map>
#include <vector>

using ShaderMap = std::map<std::string, Shader*>;

class ShaderBroker {
    ShaderBroker();
    ShaderMap            _shaders;
    static ShaderBroker* _broker;
    void                 _gatherShaderNames();
public:
    static ShaderBroker* instance();
    ~ShaderBroker();
    Shader*              getShader(std::string shaderName);
    void                 compileShaders();
    void                 recompileShader(std::string shaderName);
};