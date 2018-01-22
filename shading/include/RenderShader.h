/*
* RenderShader is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  RenderShader class. Generates the MRT position, normal and diffuse buffers
*/

#pragma once
#include "Shader.h"


//Simple shader loading class that should be derived from to create more complex shaders
class RenderShader : public Shader{

public:
    RenderShader(std::string shaderName);
    virtual      ~RenderShader();
    virtual void runShader(Model* model) = 0;
    
};
