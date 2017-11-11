/*
* Animation is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  Animation class. Stores AnimationFrames object that contains
*  the individual animation frame skin transforms within the entire
*  animation set.
*/

#pragma once
#include <string>
#include "FbxLoader.h"
#include "SkinningData.h"
#include "AnimationFrame.h"
#include "GLIncludes.h"

class Model;

class Animation {

    int                          _animationFrames;
    std::vector<AnimationFrame*> _frames;
    int                          _currentAnimationFrame;
    std::vector<GLuint>          _frameVerticesVBO;
    std::vector<GLuint>          _frameNormalsVBO;
    std::vector<GLuint>          _frameDebugNormalsVBO;
    bool                         _storedInGPU; //boolean indicating whether animation frames are prestored into GPU memory or not

public:
    Animation();
    ~Animation();
    void addFrame(AnimationFrame* frame, bool sendToGPU); //If sendToGPU is true then create a vbo for the frame
    int  getFrameCount();
    void setFrames(int animationFrames);
    void loadFrame(Model* model); //Takes in model object and assigns animation data to it
};