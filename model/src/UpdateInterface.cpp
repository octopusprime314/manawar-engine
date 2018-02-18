#include "UpdateInterface.h"
#include "SimpleContextEvents.h"
#include "ViewManager.h"

UpdateInterface::UpdateInterface() {
    //Input events
    SimpleContextEvents::subscribeToKeyboard(std::bind(&UpdateInterface::_updateKeyboard, this, _1, _2, _3));
	SimpleContextEvents::subscribeToReleaseKeyboard(std::bind(&UpdateInterface::_updateReleaseKeyboard, this, _1, _2, _3));
    SimpleContextEvents::subscribeToMouse(std::bind(&UpdateInterface::_updateMouse, this, _1, _2));
    SimpleContextEvents::subscribeToDraw(std::bind(&UpdateInterface::_updateDraw, this));
}

UpdateInterface::UpdateInterface(ViewManagerEvents* eventWrapper) {
    //Input events
    SimpleContextEvents::subscribeToKeyboard(std::bind(&UpdateInterface::_updateKeyboard, this, _1, _2, _3));
	SimpleContextEvents::subscribeToReleaseKeyboard(std::bind(&UpdateInterface::_updateReleaseKeyboard, this, _1, _2, _3));
    SimpleContextEvents::subscribeToMouse(std::bind(&UpdateInterface::_updateMouse, this, _1, _2));
    SimpleContextEvents::subscribeToDraw(std::bind(&UpdateInterface::_updateDraw, this));

    //View/Camera events
    eventWrapper->subscribeToView(std::bind(&UpdateInterface::_updateView, this, _1));
    eventWrapper->subscribeToProjection(std::bind(&UpdateInterface::_updateProjection, this, _1));
}

UpdateInterface::~UpdateInterface() {

}

void UpdateInterface::_updateView(Matrix view) {

} 
void UpdateInterface::_updateProjection(Matrix view) {

} 
