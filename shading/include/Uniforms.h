/*
* Uniforms is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  Uniforms class. Queries the uniforms and maintains them for a shader.
*  A shader class contains a Uniforms object.
*/

#pragma once
#include <map>
#include "GLIncludes.h"

struct ImageData {
    bool   readOnly;
    GLuint format;
};

class Uniforms {

    struct UniformData {
        GLint   size;     // size of the variable
        GLenum  type;     // type of the variable (float, vec3 or mat4, etc)
        GLsizei length;   // name length
        GLint   location; // location of the uniform in glsl land
    } data;

    std::map<std::string, UniformData> _uniformMap;
public:
    Uniforms(GLuint shaderContext);
    ~Uniforms();
    GLint getUniformLocation(std::string uniformName);
    void  updateUniform(std::string uniformName, void* value);
    void  updateUniform(std::string uniformName, 
        GLuint textureUnit, 
        GLuint textureContext);
    void updateUniform(std::string uniformName,
        GLuint textureUnit,
        GLuint textureContext,
        ImageData imageInfo);

};