#include "ViewManager.h"
#include <iostream>
#include "SimpleContext.h"

void ViewManager::applyTransform(Matrix transform) {
    _view = _view * transform;
}

void ViewManager::updateKeyboard(unsigned char key, int x, int y){ //Do stuff based on keyboard update
	
	if(key == 119) { //forward
		std::cout << "View manager move forward update" << std::endl;
		_view = _view * Matrix::cameraTranslation(0.0, 0.0, -0.1);
		SimpleContext::broadcastViewMatrix(_view);
	}
	else if(key == 115) { //backward
		std::cout << "View manager move backward update" << std::endl;
		_view = _view * Matrix::cameraTranslation(0.0, 0.0, 0.1);
		SimpleContext::broadcastViewMatrix(_view);
	}
}
	
void ViewManager::updateMouse(int button, int state, int x, int y){ //Do stuff based on mouse update
	
}
void ViewManager::updateDraw(){ //Do draw stuff

}
	