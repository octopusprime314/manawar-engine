/*
* AnimatedModel is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
* Copyright (c) 2017 Peter Morley.
*
* ReBoot is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, version 3.
*
* ReBoot is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/**
*  AnimatedModel class. Derives from Model class and is used for Models
*  that have animation skinning transforms.
*/

#pragma once
#include "Model.h"
#include "Animation.h"
#include "AnimationBuilder.h"
#include <mutex>

class AnimatedModel : public Model {

protected:
    //Used to tell the animated model a new animation needs to be loaded to gpu
    bool                    _animationUpdateRequest;
    int                     _currentAnimation;
    GLuint                  _weightContext;
    GLuint                  _indexContext;
    std::vector<Animation*> _animations;
    std::mutex              _updateLock;
    std::vector<Matrix>*    _currBones;

public:
    AnimatedModel(std::string name);
    ~AnimatedModel();
    void                    addAnimation(Animation* animation);
    virtual void            updateModel( Model*     model);
    void                    triggerNextFrame();
    GLuint                  getWeightContext();
    GLuint                  getIndexContext();
    void                    updateAnimation();
    //Return current animation
    Animation*              getAnimation();
    std::vector<Matrix>*    getBones();
};