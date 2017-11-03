#include "AnimationFrames.h"

AnimationFrames::AnimationFrames() {
    
}

AnimationFrames::~AnimationFrames() {
    
    for (auto skin : _skins) {
        delete skin;
    }
}

void AnimationFrames::addSkinToFrame(SkinningData* skinData) {
	_skins.push_back(skinData); //Add new skin data
}