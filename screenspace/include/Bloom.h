/*
* Bloom is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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

/*
*  Bloom class. Compute shaders that takes the deferred lighting frame
*  buffer and writes all the pixels that exceed a minimum luminance threshold.
*  That min-luminance frame buffer is then gaussian blurred horizontally and
*  vertically to create the blurry bloom effect.  The bloom frame buffer then
*  gets added on top of the deferred frame buffer.  Eventually do HDR to prevent
*  light clamping.
*/

#pragma once
class SSCompute;
class Bloom {

    SSCompute*   _luminanceFilter;
    SSCompute*   _horizontalBlur;
    SSCompute*   _verticalBlur;
public:
    Bloom();
    ~Bloom();
    unsigned int getTextureContext();
    void         compute(unsigned int deferredFBOTexture);
};