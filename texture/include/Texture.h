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
#include "GLIncludes.h"
#include <string>
#include "FreeImage.h"
#include <iostream>

class Texture{

    Texture(); //Make the default constructor private which forces coder to allocate a Texture with a string name
    void _build2DTexture(std::string textureName);
    void _buildCubeMapTexture(std::string skyboxName);
    bool _getTextureData(std::string textureName);
    void _decodeTexture(std::string textureName, unsigned int textureType);

    GLuint    _textureContext;
    int       _width;
    int       _height;
    BYTE*     _bits;
    bool      _alphaValues;
    FIBITMAP* _dib;
    std::string _name;
public:
    Texture(std::string textureName, bool cubeMap = false); //if true then it is a special cube map
    ~Texture();
    GLuint getContext();
    bool getTransparency();
};