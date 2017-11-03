#include "AnimationFrames.h"

AnimationFrames::AnimationFrames() {
    
}

void AnimationFrames::addSkinToFrame(SkinningData* skinData) {
	_skins.push_back(skinData); //Add new skin data
}