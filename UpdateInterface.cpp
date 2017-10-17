#include "UpdateInterface.h"
#include "SimpleContext.h"

UpdateInterface::UpdateInterface(){
	SimpleContext::subscribeToKeyboard(std::bind(&UpdateInterface::updateKeyboard, this, _1, _2, _3));
	SimpleContext::subscribeToMouse(std::bind(&UpdateInterface::updateMouse, this, _1, _2, _3, _4));
	SimpleContext::subscribeToDraw(std::bind(&UpdateInterface::updateDraw, this));
	SimpleContext::subscribeToView(std::bind(&UpdateInterface::updateView, this, _1));
}