#pragma once
#include "Model.h"
#include "Animation.h"
#include "AnimationBuilder.h"

class AnimatedModel : public Model {

    std::vector<Animation*> _animations;

    void updateKeyboard(unsigned char key, int x, int y); //Do stuff based on keyboard update
    void updateMouse(int button, int state, int x, int y); //Do stuff based on mouse update

public:
    AnimatedModel(ViewManagerEvents* eventWrapper);
	void addAnimation(Animation* animation);
};