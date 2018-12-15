/*
* Texture is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  The Texture class stores openGL related texture data.
*/

#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "d3dx12.h"
#include "ResourceBuffer.h"
#include "PipelineShader.h"

#include "GLIncludes.h"
#include <string>
#include "FreeImage.h"
#include <iostream>

enum class TextureFormat {
    RGBA_UNSIGNED_BYTE,
    RGBA_FLOAT,
    DEPTH32_FLOAT,
    R_FLOAT,
    R_UNSIGNED_BYTE
};

using namespace Microsoft::WRL;

class Texture {

protected:
    Texture(); //Make the default constructor private which forces coder to allocate a Texture with a string name
    GLuint                       _textureContext;
    GLuint                       _width;
    GLuint                       _height;
    std::string                  _name;
    ResourceBuffer*              _textureBuffer;
    ComPtr<ID3D12DescriptorHeap> _uavDescriptorHeap;
    ComPtr<ID3D12DescriptorHeap> _srvDescriptorHeap;
    ComPtr<ID3D12DescriptorHeap> _samplerDescriptorHeap;
    D3D12_VIEWPORT               _viewPort;
    D3D12_RECT                   _rectScissor;
public:
    Texture(std::string name);
    Texture(std::string name, GLuint width, GLuint height); 

    ~Texture();
    GLuint      getContext();
    GLuint      getWidth();
    GLuint      getHeight();
    std::string getName();
    ResourceBuffer* getResource();
    ComPtr<ID3D12DescriptorHeap> getUAVDescriptor();
    void        bindToDXShader(ComPtr<ID3D12GraphicsCommandList>& cmdList,
                               UINT textureBinding,
                               std::map<std::string, UINT>& resourceBindings);
};