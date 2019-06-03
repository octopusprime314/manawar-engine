/*
* LayeredTexture is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  The LayeredTexture class stores openGL related multiple texture data.
*/

#pragma once
#include "AssetTexture.h"
#include <vector>

class LayeredTexture {

    LayeredTexture(); //Make the default constructor private which forces coder to allocate a Texture with a string name
    std::vector<AssetTexture*> _textures;
public:
    LayeredTexture(std::vector<std::string> textureNames);
    LayeredTexture(std::vector<std::string> textureNames,
                 ComPtr<ID3D12GraphicsCommandList>& cmdList,
                 ComPtr<ID3D12Device>& device);
    ~LayeredTexture();
    std::vector<AssetTexture*> getTextures();
    void                       setTexture(AssetTexture* texture);
    void                       updateTexture(std::string textureName, FIBITMAP* pixelData);
};