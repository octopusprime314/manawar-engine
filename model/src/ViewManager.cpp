#define _USE_MATH_DEFINES
#include "ViewManager.h"
#include <iostream>
#include "SimpleContext.h"
#include "ViewManagerEvents.h"
#include "Model.h"
#include "FunctionState.h"
#include <cmath>

ViewManager::ViewManager() {

    _viewEvents = new ViewManagerEvents();
}

ViewManager::ViewManager(int* argc, char** argv, unsigned int viewportWidth, unsigned int viewportHeight) {

    //Create instance of glut wrapper class context
    //GLUT context can only run on main thread!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //PLEASE DO NOT THREAD GLUT CALLS
    _glutContext = new SimpleContext(argc, argv, viewportWidth, viewportHeight);

    _viewEvents = new ViewManagerEvents();

    _godState = true; //Start in god view mode
    _modelIndex = 0; //Start at index 0

    _thirdPersonTranslation = Matrix::cameraTranslation(0, 5, 10);
}

ViewManager::~ViewManager() {
    delete _viewEvents;
    delete _glutContext;
}

void ViewManager::run() {
    _glutContext->run();
}

void ViewManager::setProjection(unsigned int viewportWidth, unsigned int viewportHeight, float nearPlaneDistance, float farPlaneDistance) {
    //45 degree angle up/down/left/right, 
    //width by height aspect ratio
    //near plane from camera location
    //far plane from camera location
    _projection = Matrix::cameraProjection(45.0f,
        static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight),
        nearPlaneDistance,
        farPlaneDistance);

    //Broadcast perspective matrix once to all subscribers
    _viewEvents->updateProjection(_projection);


}

void ViewManager::setView(Matrix translation, Matrix rotation, Matrix scale){
    _scale = scale;
    _rotation = rotation; 
    _translation = translation;
    _view = _view * _scale * _rotation * _translation;
    _viewEvents->updateView(_view);
}

void ViewManager::setModelList(std::vector<Model*> modelList) {
    _modelList = modelList;
}

void ViewManager::applyTransform(Matrix transform) {
    _view = _view * transform;
}

ViewManagerEvents* ViewManager::getEventWrapper() {
    return _viewEvents;
}

void ViewManager::_updateReleaseKeyboard(unsigned char key, int x, int y) { //Do stuff based on keyboard release update
    //If function state exists
    if(_keyboardState.find(key) != _keyboardState.end()){
        delete _keyboardState[key];
        _keyboardState.erase(key); //erase by keyq
    }
}

void ViewManager::_updateKeyboard(unsigned char key, int x, int y) { //Do stuff based on keyboard update

    if (key == 119 || key == 115 || key == 97 || key == 100) {

        float * temp = nullptr;
        Vector4 *trans = nullptr;
		Vector4 force;
		//const float velMagnitude = 100.0f;
        const float velMagnitude = 500.0f;

        if (key == 119) { //forward w
			force = Vector4(0.0, 0.0, -velMagnitude, 1.0);
            trans = new Vector4(_inverseRotation * force); //Apply transformation based off inverse rotation
            temp = trans->getFlatBuffer();
        }
        else if (key == 115) { //backward s
			force = Vector4(0.0, 0.0, velMagnitude, 1.0);
            trans = new Vector4(_inverseRotation * force); //Apply transformation based off inverse rotation
            temp = trans->getFlatBuffer();
        }
        else if (key == 97) { //left a
			force = Vector4(-velMagnitude, 0.0, 0.0, 1.0);
            trans = new Vector4(_inverseRotation * force); //Apply transformation based off inverse rotation
            temp = trans->getFlatBuffer();
        }
        else if (key == 100) { //right d
			force = Vector4(velMagnitude, 0.0, 0.0, 1.0);
            trans = new Vector4(_inverseRotation * force); //Apply transformation based off inverse rotation
            temp = trans->getFlatBuffer();
        }

        //If not in god camera view mode then push view changes to the model for full control of a model's movements
        if (!_godState && _modelIndex < _modelList.size()) {
            
			StateVector* state = _modelList[_modelIndex]->getStateVector();
			float* pos = state->getLinearPosition().getFlatBuffer();
			_translation = Matrix::cameraTranslation(pos[0], pos[1], pos[2]); //Update the translation state matrix
            _view = _thirdPersonTranslation * _rotation * _translation; //translate then rotate around point
			
            //Define lambda equation
            auto lamdaEq = [=](double t) -> Vector4 { 
                if(t > 1.0f){
                    return static_cast<Vector4>(force);
                }
                else{
                        return static_cast<Vector4>(force) * t;
                }
                    
            };
            //lambda function container that manages force model
            //Last forever in intervals of 5 milliseconds
		    FunctionState* func = new FunctionState(std::bind(&StateVector::setForce, state, std::placeholders::_1), 
                lamdaEq, 
                5); 
            
            //Keep track to kill function when key is released
            _keyboardState[key] = func;

            //TODO rotation 
            //state->setAngularPosition(_rotation * Vector4(0, 0, 0, 0)); //Set angular (rotation) position vector based on view rotation
        }
        else{
            _translation = Matrix::cameraTranslation(temp[0], temp[1], temp[2]) * _translation; //Update the translation state matrix
            _view = _rotation * _translation; //translate then rotate around point
        }
        _viewEvents->updateView(_view); //Send out event to all listeners

        if (trans != nullptr) delete trans;
    }
    else if (key == 103) { //God's eye view change g
        _godState = true;
        _translation = Matrix::cameraTranslation(0, 5, 0); //Reset to 0,5,0 view position
        _rotation = Matrix(); //Set rotation matrix to identity
        _view = _rotation * _translation; //translate then rotate around point
        _viewEvents->updateView(_view); //Send out event to all listeners to offset locations essentially
    }
    else if (key == 113) { //Cycle through model's view point q

        _modelIndex++; //increment to the next model when q is pressed again
        if (_modelIndex >= _modelList.size()) {
            _modelIndex = 0;
        }

        _godState = false;
        StateVector* state = _modelList[_modelIndex]->getStateVector();
        float* position = state->getLinearPosition().getFlatBuffer();
        float* rotation = state->getAngularPosition().getFlatBuffer();
        _translation = Matrix::cameraTranslation(position[0], position[1], position[2]); //Set camera to model's world space translation
        Matrix rotY = Matrix::cameraRotationAroundY(rotation[1]); //Set rotation around Y
        Matrix rotX = Matrix::cameraRotationAroundX(rotation[0]); //Set rotation around X
        Matrix rotZ = Matrix::cameraRotationAroundZ(rotation[2]); //Set rotation around Z
        _rotation = rotZ * rotX * rotY;

        //Last transform to be applied to achieve third person view
        _view = _thirdPersonTranslation * _rotation * _translation; //translate then rotate around point
        _viewEvents->updateView(_view); //Send out event to all listeners to offset locations essentially
        
    }
}

void ViewManager::_updateMouse(int button, int state, int x, int y) { //Do stuff based on mouse update

    int widthMidpoint = 1920 / 2;
    int heightMidpoint = 1080 / 2;

    if (x < widthMidpoint || x > widthMidpoint) {
        if (x < widthMidpoint) { //rotate left around y axis
            _rotation = _rotation * Matrix::cameraRotationAroundY(0.4); //Update the rotation state matrix
            _inverseRotation = _inverseRotation * Matrix::cameraRotationAroundY(-0.4); //Inverse rotation for translation updates
        }
        else if (x > widthMidpoint) { //rotate right around y axis
            _rotation = _rotation * Matrix::cameraRotationAroundY(-0.4); //Update the rotation state matrix
            _inverseRotation = _inverseRotation * Matrix::cameraRotationAroundY(0.4); //Inverse rotation for translation updates
        }

        _view = _rotation * _translation; //translate then rotate around point
        //If not in god camera view mode then push view changes to the model for full control of a model's movements
        if (!_godState){
            _view = _thirdPersonTranslation * _view;
        }
        _viewEvents->updateView(_view); //Send out event to all listeners
        glutWarpPointer(widthMidpoint, heightMidpoint);  //Bring cursor back to center position
    }
}
void ViewManager::_updateDraw() { //Do draw stuff

	//If not in god camera view mode then push view changes to the model for full control of a model's movements
    if (!_godState && _modelIndex < _modelList.size()) {
            
		StateVector* state = _modelList[_modelIndex]->getStateVector();
		float* pos = state->getLinearPosition().getFlatBuffer();
		_translation = Matrix::cameraTranslation(pos[0], pos[1], pos[2]); //Update the translation state matrix

		 //Last transform to be applied to achieve third person view
        _view = _thirdPersonTranslation * _rotation * _translation; //translate then rotate around point
        _viewEvents->updateView(_view); //Send out event to all listeners to offset locations essentially

        //TODO rotation 
        //state->setAngularPosition(_rotation * Vector4(0, 0, 0, 0)); //Set angular (rotation) position vector based on view rotation
    }
}
