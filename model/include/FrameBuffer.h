/*
* FrameBuffer is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  FrameBuffer class. Stores framebuffer color, depth or stencil buffer data
   into a texture format for post processing and offscreen rendering
   The texture object is used for the color attachment and the render buffer
   object is used for depth attachment
*/

#pragma once
#include "GLIncludes.h"
#include <iostream>

class FrameBuffer {

    GLuint _frameBufferContext;
    GLuint _fbTextureContext;

public:
    FrameBuffer();
    ~FrameBuffer();
    GLuint getFrameBufferContext();
    GLuint getTextureContext();
};