#include "AnimatedModel.h"

AnimatedModel::AnimatedModel(std::string name, ViewManagerEvents* eventWrapper) : Model(name, eventWrapper, ModelClass::AnimatedModelType) {

    _fbxLoader->loadAnimatedModel(this, _fbxLoader->getScene()->GetRootNode()); //Grab skin animation transforms

    //Done with fbx loader so close all handles to fbx scene instance
    delete _fbxLoader;
}

AnimatedModel::~AnimatedModel() {

    for (auto animation : _animations) {
        delete animation;
    }
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
