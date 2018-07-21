/*
* RenderBuffers is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  RenderBuffers class. Manages the vertices, normals and texture data
*/

#pragma once
#include <vector>
#include "Vector4.h"
#include "Tex2.h"

class RenderBuffers {

    std::vector<Vector4>        _vertices; //Vertices that make up the triangles of the model
    std::vector<Vector4>        _normals; //Normals that implement how light is shaded onto a model
    std::vector<Tex2>           _textures; //Texture coordinates that places texture data and maps it onto a vertex
    std::vector<int>            _indices; //Used to map vertices
    std::vector<Vector4>        _debugNormals; //Vertex storage for normal line visualization

public:
    RenderBuffers();
    ~RenderBuffers();
    std::vector<Vector4>*       getVertices();
    std::vector<Vector4>*       getNormals();
    std::vector<Tex2>*          getTextures();
    std::vector<int>*           getIndices();
    std::vector<Vector4>*       getDebugNormals();
    void                        addVertex(Vector4 vertex);
    void                        addNormal(Vector4 normal);
    void                        addTexture(Tex2 texture);
    void                        addDebugNormal(Vector4 normal);
    void                        setVertexIndices(std::vector<int> indices);
    void                        addVertexIndices(std::vector<int> indices);
    void                        clearBuffers();
};