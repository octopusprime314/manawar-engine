#include "AnimatedModel.h"

AnimatedModel::AnimatedModel(std::string name, ViewManagerEvents* eventWrapper) :
    Model(name, eventWrapper, ModelClass::AnimatedModelType),
    _currentAnimation(0) {

    _fbxLoader->loadAnimatedModel(this, _fbxLoader->getScene()->GetRootNode()); //Grab skin animation transforms

    //Done with fbx loader so close all handles to fbx scene instance
    delete _fbxLoader;

    //Hook up to framerate update for proper animation progression
    _clock->subscribeFrameRate(std::bind(&AnimatedModel::_updateAnimation, this, std::placeholders::_1)); 
}

AnimatedModel::~AnimatedModel() {

    for (auto animation : _animations) {
        delete animation;
    }
}

void AnimatedModel::_updateDraw() {

    //If animation update request was received from an asynchronous event then send vbo to gpu
    if(_animationUpdateRequest){
        //Swap buffer context so the next animation gpu data will be in the gpu when requested
        _doubleBufferIndex = !_doubleBufferIndex;

        //Load in the next frame of the animation
        _animations[_currentAnimation]->loadFrame(this);
        _updateLock.lock(); _animationUpdateRequest = false; _updateLock.unlock();
    }

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

void AnimatedModel::_updateAnimation(int milliSeconds){
    //Coordinate loading new animation frame to gpu
    _updateLock.lock(); _animationUpdateRequest = true; _updateLock.unlock();
}