#include "Animation.h"

Animation::Animation(){

}

void Animation::addSkin(SkinningData* skin){
	_frames.addSkinToFrame(skin);
}