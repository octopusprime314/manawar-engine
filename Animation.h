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
class Animation {

    int                          _animationFrames;
    std::vector<AnimationFrame*> _frames;
    int                          _currentAnimationFrame;

public:
    Animation();
    ~Animation();
    void addFrame(AnimationFrame* frame);
    int getFrameCount();
    AnimationFrame* getNextFrame();
    void setFrames(int animationFrames);
};