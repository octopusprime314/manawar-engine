/*
* SkyBox is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  SkyBox class. Displays far away background scenes and derives from CubeMap class
*/

#pragma once
#include "CubeMap.h"
#include "GLIncludes.h"
#include "TextureBroker.h"  
#include "MVP.h"
#include "VBO.h"
#include "SkyboxShader.h"

class SkyBox {

    MVP _mvp;
    VBO _vbo;
    SkyboxShader _skyboxShader;
    Texture* _cubeMapTexture;
public:
    SkyBox(std::string folder);
    ~SkyBox();
    MVP* getMVP();
    VBO* getVBO();
    Texture* getCubeMapTexture();
};