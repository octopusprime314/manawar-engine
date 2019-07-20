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
#include "Animation.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Cube.h"
#include <d3d12.h>
#include "d3dx12.h"
#include <wrl.h>
#include "ResourceBuffer.h"

using namespace Microsoft::WRL;

enum class GeometryConstruction;
enum class ModelClass;
using      TextureMetaData = std::vector<std::pair<std::string, int>>;
class VAO {

    void                     _buildVertices(float* flattenVerts);
    GLuint                   _debugNormalBufferContext;
    GLuint                   _textureBufferContext;
    GLuint                   _vertexBufferContext;
    GLuint                   _normalBufferContext;
    GLuint                   _indexBufferContext;
    GLuint                   _primitiveOffsetId;
    GLuint                   _vaoShadowContext;
    GLuint                   _weightContext;
    TextureMetaData          _textureStride;
    GLuint                   _indexContext;
    GLuint                   _vertexLength;
    ResourceBuffer*          _vertexBuffer;
    ResourceBuffer*          _indexBuffer;
    GLuint                   _vaoContext;
    D3D12_INDEX_BUFFER_VIEW  _ibv;
    D3D12_VERTEX_BUFFER_VIEW _vbv;
public:

    VAO();
    ~VAO();
    VAO(D3D12_VERTEX_BUFFER_VIEW vbv,
        D3D12_INDEX_BUFFER_VIEW ibv);

    void                     addTextureStride(std::pair<std::string, int> stride);
    void                     createVAO(std::vector<Triangle>* triangles);
    void                     createVAO(std::vector<Sphere>* spheres,
                                       GeometryConstruction geometryType);
    void                     createVAO(std::vector<Cube>* cubes,
                                       GeometryConstruction geometryType);
    void                     createVAO(RenderBuffers* renderBuffers,
                                       ModelClass classId,
                                       Animation* = nullptr);
    void                     createVAO(RenderBuffers* renderBuffers,
                                       int begin,
                                       int range);

    void                     setNormalDebugContext(GLuint context);
    void                     setTextureContext(GLuint context);
    void                     setVertexContext(GLuint context);
    void                     setNormalContext(GLuint context);
    void                     setPrimitiveOffsetId(GLuint id);
    GLuint                   getNormalDebugContext();
    GLuint                   getPrimitiveOffsetId();
    GLuint                   getVAOShadowContext();
    TextureMetaData          getTextureStrides();
    GLuint                   getTextureContext();
    ResourceBuffer*          getVertexResource();
    ResourceBuffer*          getIndexResource();
    GLuint                   getNormalContext();
    GLuint                   getVertexContext();
    D3D12_VERTEX_BUFFER_VIEW getVertexBuffer();
    GLuint                   getVertexLength();
    D3D12_INDEX_BUFFER_VIEW  getIndexBuffer();
    GLuint                   getVAOContext();

    
};