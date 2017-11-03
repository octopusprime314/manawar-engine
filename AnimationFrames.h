#pragma once
#include "Matrix.h"
#include "SkinningData.h"
#include <vector>
#include <string>

const float FRAMERATE = 60.0f;

class AnimationFrames {
    
    int                        _animationFrames;
    std::vector<SkinningData*> _skins;

public:
    AnimationFrames();
    void addSkinToFrame(SkinningData* skinData);
};