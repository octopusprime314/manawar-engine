#pragma once
#include <functional>
#include "Matrix.h"
using namespace std::placeholders; 

//Class needed to derive from to subscribe to input events
class UpdateInterface{

public:
	UpdateInterface();
	virtual ~UpdateInterface(){}

protected:
	virtual void updateKeyboard(unsigned char key, int x, int y) = 0;
	virtual void updateMouse(int button, int state, int x, int y) = 0;
	virtual void updateDraw() = 0;
	virtual void updateView(Matrix view){} //optional to implement overriden function
};