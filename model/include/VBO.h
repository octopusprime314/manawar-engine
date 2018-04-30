/*
* VBO is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  VBO class. Vertex Buffer Object container
*/

#pragma once
#include "GLIncludes.h"
#include <vector>
#include "Vector4.h"
#include "Tex2.h"
#include "RenderBuffers.h"

enum class ModelClass; //Forward declaration of enumerated type while not including Model class

class VBO {

    GLuint  _vertexBufferContext; //Used as the vertex attribute vbo context
    GLuint  _normalBufferContext; //Used as the normal attribute vbo context
    GLuint  _textureBufferContext; //Used as the texture coordinate attribute vbo context
    GLuint  _debugNormalBufferContext; //Used as the debug normal line attribute vbo context

public:
    VBO();
    ~VBO();
    GLuint  getVertexContext();
    GLuint  getNormalContext();
    GLuint  getTextureContext();
    GLuint  getNormalDebugContext();
    void    setVertexContext(GLuint context);
    void    setNormalContext(GLuint context);
    void    setTextureContext(GLuint context);
    void    setNormalDebugContext(GLuint context);
    void    createVBO(RenderBuffers* renderBuffers, ModelClass classId);
};