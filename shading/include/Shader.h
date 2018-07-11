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
*  Shader class. Basic shader class that is responsible only for compilation and
*  storing the gl context of the shader
*/

#pragma once
#include <string>
#include <iostream>
#include "GLIncludes.h"
#include "Uniforms.h"

class Entity;
class Light;
class ShaderBroker;

const std::string SHADERS_LOCATION = "../shading/shaders/";

//Simple shader loading class that should be derived from to create more complex shaders
class Shader {

protected:
    GLuint       _shaderContext; //keeps track of the shader context
    std::string  _vertexShaderName;
    std::string  _fragmentShaderName;
    Uniforms*    _uniforms;
    unsigned int _compile(char* filename, unsigned int type);
    void         _link(unsigned int vertexSH, unsigned int fragmentSH,
                       unsigned int geomSH, unsigned int computeSH);
    void         _build();

public:
    Shader(std::string vertexShaderName, std::string fragmentShaderName = "");
    Shader(const Shader& shader);
    virtual      ~Shader();
    GLint        getShaderContext();
    GLint        getLocation(std::string uniformName);
    Uniforms*    getUniforms();
    //Updates most mat, vec, etc. data types in glsl
    void         updateUniform(std::string uniformName, 
                    void* data);
    //Updates samplers in glsl
    void         updateUniform(std::string uniformName, 
                    GLuint textureUnit, 
                    GLuint textureContext);
    //Updates images in glsl used primarily in compute shaders
    void         updateUniform(std::string uniformName,
                    GLuint textureUnit,
                    GLuint textureContext,
                    ImageData imageInfo);
    void         updateShader(Shader* shader);

};