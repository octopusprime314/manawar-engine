/*
* CubeMap is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
* Copyright (c) 2018 Peter Morley.
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
*  CubeMap class. Generic cube map that can be used for point shadows, skybox or reflection maps.
*/

#pragma once
#include <vector>
#include "Vector4.h"
#include "Matrix.h"

class CubeMap {

protected:
    unsigned int _depthCubemap; //Cube texture reference
    unsigned int _cubeFrameBuffer; //Cube frame buffer used for render to cube texture
    //Width and height are used to change viewport when rendering
    unsigned int _width; //Width of texture 
    unsigned int _height; //Height of texture
public:
    CubeMap(unsigned int width, unsigned int height);
    ~CubeMap();

    unsigned int getWidth();
    unsigned int getHeight();
    unsigned int getCubeDepthFrameBufferContext();
    unsigned int getCubeDepthMapContext();
};

