/*
* ComputeShader is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
* Copyright (c) 2018 Peter Morley.
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
*  ComputeShader class. Takes in a texture and outputs a texture that is based on the shader
*  implementation.  This class should have more generic member variables.
*/

#pragma once
#include "Shader.h"

enum class Format {
    RGBUB = 0, //RGB Unsigned Bytes
    RGBF,      //RGB Floats
    RU,        //RU  Unsigned Byte
    RF,        //RF  Float
};

class ComputeShader : public Shader {

    unsigned int _readTextureLocation;
    unsigned int _writeTextureLocation;
public:
    ComputeShader(std::string computeShaderName);
    ~ComputeShader();
    void         runShader(GLuint writeTexture, GLuint readTexture, Format format);
};