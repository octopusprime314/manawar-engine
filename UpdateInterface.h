#pragma once
#include "SimpleContext.h"
#include <functional>
using namespace std::placeholders; 

//Class needed to derive from to subscribe to input events
class UpdateInterface{

public:
	UpdateInterface(){
		SimpleContext::subscribeToKeyboard(std::bind(&UpdateInterface::updateKeyboard, this, _1, _2, _3));
		SimpleContext::subscribeToMouse(std::bind(&UpdateInterface::updateMouse, this, _1, _2, _3, _4));
	}
	virtual ~UpdateInterface(){}

protected:
	virtual void updateKeyboard(unsigned char key, int x, int y) = 0;
	virtual void updateMouse(int button, int state, int x, int y) = 0;
};