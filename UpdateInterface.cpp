#include "UpdateInterface.h"
#include "SimpleContext.h"
#include "ViewManagerEvents.h"

UpdateInterface::UpdateInterface(){
	//Input events
	SimpleContext::subscribeToKeyboard(std::bind(&UpdateInterface::updateKeyboard, this, _1, _2, _3));
	SimpleContext::subscribeToMouse(std::bind(&UpdateInterface::updateMouse, this, _1, _2, _3, _4));
	SimpleContext::subscribeToDraw(std::bind(&UpdateInterface::updateDraw, this));

	//View/Camera events
	ViewManagerEvents::subscribeToView(std::bind(&UpdateInterface::updateView, this, _1));
	ViewManagerEvents::subscribeToProjection(std::bind(&UpdateInterface::updateProjection, this, _1));
}