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
#include "ShaderBase.h"
#include <map>
#include <vector>
#include <wrl.h>
#include <d3d12.h>
#include "d3dx12.h"
#include "PresentTarget.h"

using namespace Microsoft::WRL;

using ShaderMap = std::map<std::string, ShaderBase*>;

class ShaderBroker {
    ShaderBroker();

    std::string          _strToUpper(std::string s);
    void                 _gatherShaderNames();
    ShaderMap            _shaders;
    static ShaderBroker* _broker;
public:
    ~ShaderBroker();
    void                 recompileShader(std::string shaderName);
    ShaderBase*          getShader(      std::string shaderName);
    void                 compileShaders();
    static ShaderBroker* instance();

};