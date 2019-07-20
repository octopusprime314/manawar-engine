/*
* AssetTexture is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  The AssetTexture class opens and stores textures associated with loaded models
*/

#pragma once
#include "Texture.h"
 
class AssetTexture : public Texture{

    //Make the default constructor private which forces coder to allocate a Texture with a string name
    AssetTexture();

    void         _build2DTextureDX(     std::string textureName,
                                        ComPtr<ID3D12GraphicsCommandList>& cmdList,
                                        ComPtr<ID3D12Device>& device);

    void         _build2DTextureDX(     void* data,
                                        UINT width,
                                        UINT height,
                                        ComPtr<ID3D12GraphicsCommandList>& cmdList,
                                        ComPtr<ID3D12Device>& device);

    void         _buildCubeMapTextureDX(std::string skyboxName,
                                        ComPtr<ID3D12GraphicsCommandList>& cmdList,
                                        ComPtr<ID3D12Device>& device);

    void         _build2DTextureGL(     void* data,
                                        UINT width,
                                        UINT height);

    void         _decodeTexture(        std::string textureName,
                                        uint32_t textureType,
                                        uint32_t target);

    void         _buildCubeMapTextureGL(std::string skyboxName);
    void         _build2DTextureGL(std::string textureName);
    bool         _getTextureData(std::string textureName);
  
    unsigned int _imageBufferSize;
    bool         _alphaValues;
    BYTE*        _bits;
    FIBITMAP*    _dib;

public:
    AssetTexture(std::string textureName,
                 bool cubeMap = false);

    AssetTexture(std::string textureName, 
                 ComPtr<ID3D12GraphicsCommandList>& cmdList,
                 ComPtr<ID3D12Device>& device,
                 bool cubeMap = false);

    AssetTexture(void* data, UINT width, UINT height,
                 ComPtr<ID3D12GraphicsCommandList>& cmdList,
                 ComPtr<ID3D12Device>& device);

    AssetTexture(void* data,
                 UINT width,
                 UINT height);

    ~AssetTexture();

    void  updateTexture(FIBITMAP* data);
    bool  getTransparency();
    void  buildMipLevels();
    BYTE* getBits();
};