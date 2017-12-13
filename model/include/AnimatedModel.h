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
    void                    _updateKeyboard(unsigned char key, int x, int y); //Do stuff based on keyboard update
    void                    _updateMouse(int button, int state, int x, int y); //Do stuff based on mouse update
    void                    _updateDraw();
    std::vector<Animation*> _animations;
    int                     _currentAnimation;
    void                    _updateAnimation(int milliSeconds);
    bool                    _animationUpdateRequest; //Used to tell the animated model a new animation needs to be loaded to gpu
    std::mutex              _updateLock;

    GLuint                  _indexContext;
    GLuint                  _weightContext;
    std::vector<Matrix>*    _currBones;


public:
    AnimatedModel(std::string name, ViewManagerEvents* eventWrapper);
    ~AnimatedModel();
    void                    addAnimation(Animation* animation);
    Animation*              getAnimation(); //Return current animation
    GLuint                  getIndexContext();
    GLuint                  getWeightContext();
    std::vector<Matrix>*    getBones();
};