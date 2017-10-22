#include "ViewManager.h"
#include <iostream>
#include "SimpleContext.h"
#include "ViewManagerEvents.h"

ViewManager::ViewManager() {
	_viewEvents = new ViewManagerEvents();
}

void ViewManager::setProjection(){
	_projection =  Matrix::cameraProjection(45.0f, 1080.0f/1920.0f, 0.1, 100.0); //45 degree angle up/down/left/right, 
																				 //1080/1920 aspect ratio
																				 //near plane from camera location
																				 //far plane from camera location
	//Broadcast perspective matrix once to all subscribers
	_viewEvents->updateProjection(_projection);
}

void ViewManager::applyTransform(Matrix transform) {
    _view = _view * transform;
}

ViewManagerEvents* ViewManager::getEventWrapper(){
	return _viewEvents;
}

void ViewManager::updateKeyboard(unsigned char key, int x, int y){ //Do stuff based on keyboard update
	
	if(key == 119 || key == 115 || key == 97 || key == 100) {
		
		float * temp = nullptr;
		Vector4 *trans = nullptr;

		if(key == 119) { //forward
			trans = new Vector4(_inverseRotation * Vector4(0.0, 0.0, -0.1, 1.0)); //Apply transformation based off inverse rotation
			temp = trans->getFlatBuffer();
		}
		else if(key == 115) { //backward
			trans = new Vector4(_inverseRotation * Vector4(0.0, 0.0, 0.1, 1.0)); //Apply transformation based off inverse rotation
			temp = trans->getFlatBuffer();
		}
		else if(key == 97) { //left
			trans = new Vector4(_inverseRotation * Vector4(-0.1, 0.0, 0.0, 1.0)); //Apply transformation based off inverse rotation
			temp = trans->getFlatBuffer();
		}
		else if(key == 100) { //right
			trans = new Vector4(_inverseRotation * Vector4(0.1, 0.0, 0.0, 1.0)); //Apply transformation based off inverse rotation
			temp = trans->getFlatBuffer();
		}

		_translation = Matrix::cameraTranslation(temp[0], temp[1], temp[2]) * _translation; //Update the translation state matrix
		_view = _rotation * _translation; //translate then rotate around point
		_viewEvents->updateView(_view); //Send out event to all listeners

		if(trans != nullptr) delete trans;
	}
}
	
void ViewManager::updateMouse(int button, int state, int x, int y){ //Do stuff based on mouse update
	
	int widthMidpoint = 1080/2;
	int heightMidpoint = 1920/2;
	
	if(x < widthMidpoint || x > widthMidpoint) {
		if(x < widthMidpoint) { //rotate left around y axis
			_rotation = _rotation * Matrix::cameraRotationAroundY(0.2); //Update the rotation state matrix
			_inverseRotation = _inverseRotation * Matrix::cameraRotationAroundY(-0.2); //Inverse rotation for translation updates
		}
		else if(x > widthMidpoint) { //rotate right around y axis
			_rotation = _rotation * Matrix::cameraRotationAroundY(-0.2); //Update the rotation state matrix
			_inverseRotation = _inverseRotation * Matrix::cameraRotationAroundY(0.2); //Inverse rotation for translation updates
		}

		_view = _rotation * _translation; //translate then rotate around point
		_viewEvents->updateView(_view); //Send out event to all listeners
		glutWarpPointer(widthMidpoint,heightMidpoint);  //Bring cursor back to center position
	}
}
void ViewManager::updateDraw(){ //Do draw stuff

}
	