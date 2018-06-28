/*
* TextureBroker is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  The TextureBroker class is a singleton that manages all textures in a scene
*/

#pragma once
#include "AssetTexture.h"
#include "LayeredTexture.h"
#include <map>
#include <vector>

using LayeredTextureMap = std::map<std::string, LayeredTexture*>;
using TextureMap = std::map<std::string, AssetTexture*>;

class TextureBroker {
    TextureBroker();
    TextureMap                      _textures;
    LayeredTextureMap               _layeredTextures;
    static TextureBroker*           _broker;
public:
    static TextureBroker*           instance();
    ~TextureBroker();
    void                            addTexture(std::string textureName);
    void                            addLayeredTexture(std::vector<std::string> textureNames);
    void                            addCubeTexture(std::string textureName);
    AssetTexture*                   getTexture(std::string textureName);
    LayeredTexture*                 getLayeredTexture(std::string textureName);
};