#include "Animation.h"

Animation::Animation() : _currentAnimationFrame(0) {

}

Animation::~Animation() {

}

void Animation::addFrame(AnimationFrame* frame) {
    _frames.push_back(frame);
}

int Animation::getFrameCount() {
    return _animationFrames;
}

AnimationFrame* Animation::getNextFrame() {
    if (_currentAnimationFrame >= _frames.size()) {
        _currentAnimationFrame = 0;
    }
    return _frames[_currentAnimationFrame++];
}

void Animation::setFrames(int animationFrames) {
    _animationFrames = animationFrames;
}