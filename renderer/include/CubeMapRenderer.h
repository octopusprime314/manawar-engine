/*
* CubeMapRenderer is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  CubeMapRenderer class. Uses a cube map texture from a position in space and renders
*  all six model view projection transforms onto each face of the cube map
*/

#pragma once

#include "CubeMap.h"
#include <vector>
#include "Light.h"
#include "Model.h"
#include "ShadowAnimatedPointShader.h"

class CubeMapRenderer {

protected:
    CubeMap             _cubeTextureMap; //Contains 6 size for point light shadow rendering
    bool                _isDepth; //Depth or color buffer cube map renderer
    std::vector<Matrix> _transforms;
public:
    CubeMapRenderer(GLuint width, GLuint height, bool isDepth);
    ~CubeMapRenderer();

    void                preCubeFaceRender(std::vector<Model*> modelList, MVP* mvp);
    void                postCubeFaceRender();
    GLuint              getCubeMapTexture();
};
