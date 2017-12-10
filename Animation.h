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
#include "GLIncludes.h"

class Model;

class Animation {

    int                                _animationFrames;
    int                                _currentAnimationFrame;
    std::vector<std::vector<int>>*     _boneIndexes;
    std::vector<std::vector<float>>*   _boneWeights;
    std::vector<std::vector<Matrix>*>  _boneTransforms;
    std::vector<SkinningData>          _skinData;

public:
    Animation();
    ~Animation();
    
    int  getFrameCount();
    void setFrames(int animationFrames);
    void nextFrame(); //set to the next animation frame
    void setBoneIndexWeights( std::vector<std::vector<int>>* boneIndexes, std::vector<std::vector<float>>* boneWeights); 
    void addBoneTransforms(std::vector<Matrix>* boneTransforms); 
    std::vector<Matrix>* getBones();
    std::vector<std::vector<int>>* getBoneIndexes();
    std::vector<std::vector<float>>* getBoneWeights();
    void addSkin(std::vector<SkinningData> skinData);
    std::vector<SkinningData>& getSkins();
};