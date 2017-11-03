#pragma once
#include <string>
#include "AnimationFrames.h"
#include "FbxLoader.h"
class Animation {

    AnimationFrames _frames; //Contains the skin/bone transforms that compose an entire animation frame

public:
	Animation();
	void addSkin(SkinningData* skin);
};