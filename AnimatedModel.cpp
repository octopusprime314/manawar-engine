#include "AnimatedModel.h"

AnimatedModel::AnimatedModel(std::string name, ViewManagerEvents* eventWrapper) : Model(name, eventWrapper) {

	//Load in fbx object
    FbxLoader fbxLoad(name);
	fbxLoad.loadAnimatedModel(this, fbxLoad.getScene()->GetRootNode()); //Grab skin animation transformss
}

void AnimatedModel::updateKeyboard(unsigned char key, int x, int y) {
    //Update animation 

    //Call base class keyboard handling
    Model::updateKeyboard(key, x, y);
}

void AnimatedModel::updateMouse(int button, int state, int x, int y) {
    //Invoke an animation 

    //Call base class keyboard handling
    Model::updateMouse(button, state, x, y);
}

void AnimatedModel::addAnimation(Animation* animation){
	_animations.push_back(animation);
}
