#include "UpdateInterface.h"
#include "SimpleContextEvents.h"
#include "ViewManager.h"

UpdateInterface::UpdateInterface(){
	//Input events
	SimpleContextEvents::subscribeToKeyboard(std::bind(&UpdateInterface::updateKeyboard, this, _1, _2, _3));
	SimpleContextEvents::subscribeToMouse(std::bind(&UpdateInterface::updateMouse, this, _1, _2, _3, _4));
	SimpleContextEvents::subscribeToDraw(std::bind(&UpdateInterface::updateDraw, this));
}

UpdateInterface::UpdateInterface(ViewManagerEvents* eventWrapper){
	//Input events
	SimpleContextEvents::subscribeToKeyboard(std::bind(&UpdateInterface::updateKeyboard, this, _1, _2, _3));
	SimpleContextEvents::subscribeToMouse(std::bind(&UpdateInterface::updateMouse, this, _1, _2, _3, _4));
	SimpleContextEvents::subscribeToDraw(std::bind(&UpdateInterface::updateDraw, this));

	//View/Camera events
	eventWrapper->subscribeToView(std::bind(&UpdateInterface::updateView, this, _1));
	eventWrapper->subscribeToProjection(std::bind(&UpdateInterface::updateProjection, this, _1));
}

UpdateInterface::~UpdateInterface() {

}