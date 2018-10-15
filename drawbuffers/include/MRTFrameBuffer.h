/*
* MRTFrameBuffer is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  MRTFrameBuffer class. Handles multiple render targets and creates multiple color
*  attachments to a single frame buffer object
*/

#pragma once
#include "RenderTexture.h"
#include "GLIncludes.h"
#include <iostream>
#include <vector>

class MRTFrameBuffer {

    GLuint                      _frameBufferContext;
    std::vector<RenderTexture>  _gBufferTextures;
    std::vector<GLuint>         _fbTextureContexts; //Use for color attachments in framebuffer

public:
    MRTFrameBuffer();
    ~MRTFrameBuffer();
    GLuint                      getFrameBufferContext();
    std::vector<GLuint>         getTextureContexts();
    std::vector<RenderTexture>& getTextures();
    void bind();
    void unbind();
};