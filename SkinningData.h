#pragma once
#include <vector>
#include "Matrix.h"
#include "fbxsdk.h"
class SkinningData {
 
    std::vector<int>          _indexes;
    std::vector<double>       _weights;
    std::vector<Matrix>       _frameVertexTransforms;
    std::vector<Matrix>       _frameNormalTransforms;

public:
    SkinningData(FbxCluster* skinData, FbxNode* node, int animationFrames);
};