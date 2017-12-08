#include "Animation.h"
#include "Model.h"
Animation::Animation() : _currentAnimationFrame(0) {

}

Animation::~Animation() {

}

void Animation::nextFrame(){
    
    _currentAnimationFrame++; //Increment the animation frame counter for next call

    if (_currentAnimationFrame >= _boneTransforms.size()) {
        _currentAnimationFrame = 0;
    }
}

int Animation::getFrameCount() {
    return _animationFrames;
}
void Animation::setFrames(int animationFrames) {
    _animationFrames = animationFrames;
}

void Animation::setBoneIndexWeights( std::vector<std::vector<int>>* boneIndexes, std::vector<std::vector<float>>* boneWeights){
    _boneIndexes = boneIndexes;
    _boneWeights = boneWeights;
}

void Animation::addBoneTransforms(std::vector<Matrix>* boneTransforms){
    _boneTransforms.push_back(boneTransforms);
}

std::vector<Matrix>* Animation::getBones() {
    return _boneTransforms[_currentAnimationFrame];
}
    
std::vector<std::vector<int>>* Animation::getBoneIndexes() {
    return _boneIndexes;
}
    
std::vector<std::vector<float>>* Animation::getBoneWeights() {
    return _boneWeights;
}
