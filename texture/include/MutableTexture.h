/*
* MutableTexture is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  The MutableTexture class edits and can create new textures for model development
*/

#pragma once
#include "Texture.h"
#include "AssetTexture.h"
#include "Vector4.h"

class MutableTexture : public Texture{

    MutableTexture(); //Make the default constructor private which forces coder to allocate a Texture with a string name
    void      _createTextureData();
    void      _cloneTexture(std::string newName);
    FIBITMAP* _bitmapToWrite;
public:
    MutableTexture(std::string textureName); //texture already exists!
    MutableTexture(std::string originalTexture, std::string clonedTexture);
    MutableTexture(std::string textureName, int width, int height); 
    ~MutableTexture();
    void editTextureData(int x, int y, Vector4 texturePixel, int radius = 0);
    void saveToDisk();
};