/*
* DeferredFrameBuffer is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  DeferredFrameBuffer class. Handles multiple render targets and creates multiple color
*  attachments to a single frame buffer object
*/

#pragma once
#include "GLIncludes.h"
#include "RenderTexture.h"
#include <iostream>
#include <vector>

class DeferredFrameBuffer {

    GLuint        _frameBufferContext;
    RenderTexture _renderTexture;
    RenderTexture _depthTexture;

public:
    DeferredFrameBuffer();
    ~DeferredFrameBuffer();
    GLuint        getFrameBufferContext();
    GLuint        getTextureContext();
    Texture*      getDepthTexture();
    Texture*      getRenderTexture();

};