/*
* SkinningData is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  SkinningData class. Stores frame data for animation transforms.
*/

#pragma once
#include <vector>
#include "Matrix.h"
#include "fbxsdk.h"
class SkinningData {

    std::vector<int>          _indexes;
    std::vector<float>        _weights;
    std::vector<Matrix>       _frameVertexTransforms;
    int                       _indexOffset;

public:
    SkinningData(FbxCluster* skinData, FbxNode* node, int animationFrames, int indexOffset);
    ~SkinningData();

    std::vector<int>*          getIndexes();
    std::vector<float>*        getWeights();
    std::vector<Matrix>*       getFrameVertexTransforms();
    int                        getIndexOffset();
};