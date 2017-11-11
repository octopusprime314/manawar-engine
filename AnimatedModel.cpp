#include "AnimatedModel.h"

AnimatedModel::AnimatedModel(std::string name, ViewManagerEvents* eventWrapper) :
    Model(name, eventWrapper, ModelClass::AnimatedModelType),
    _currentAnimation(0) {

    _fbxLoader->loadAnimatedModel(this, _fbxLoader->getScene()->GetRootNode()); //Grab skin animation transforms

    //Done with fbx loader so close all handles to fbx scene instance
    delete _fbxLoader;
}

AnimatedModel::~AnimatedModel() {

    for (auto animation : _animations) {
        delete animation;
    }
}

void AnimatedModel::_updateDraw() {

    //Load in the next frame of the animation
    _animations[_currentAnimation]->loadFrame(this);

    Model::_updateDraw();
}

void AnimatedModel::_updateKeyboard(unsigned char key, int x, int y) {
    //Update animation 

    //Call base class keyboard handling
    Model::_updateKeyboard(key, x, y);
}

void AnimatedModel::_updateMouse(int button, int state, int x, int y) {
    //Invoke an animation 

    //Call base class keyboard handling
    Model::_updateMouse(button, state, x, y);
}

void AnimatedModel::addAnimation(Animation* animation) {
    _animations.push_back(animation);
}

Animation* AnimatedModel::getAnimation() {
    return _animations.back();
}