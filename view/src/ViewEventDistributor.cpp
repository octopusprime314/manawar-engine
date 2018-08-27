#define _USE_MATH_DEFINES
#include "ViewEventDistributor.h"
#include <iostream>
#include "ViewEvents.h"
#include "Model.h"
#include "FunctionState.h"
#include <cmath>
#include "StateVector.h"
#include "Entity.h"
#include "ShaderBroker.h"
#include "Matrix.h"

ViewEventDistributor::ViewEventDistributor() {

    _viewEvents = new ViewEvents();
}

ViewEventDistributor::ViewEventDistributor(int* argc, char** argv, unsigned int viewportWidth, unsigned int viewportHeight) {

    _viewEvents = new ViewEvents();

    _godState = true; //Start in god view mode
    _entityIndex = 0; //Start at index 0

    _thirdPersonTranslation = Matrix::cameraTranslation(0, 5, 10);

    _prevMouseX = IOEventDistributor::screenPixelWidth / 2;
    _prevMouseY = IOEventDistributor::screenPixelHeight / 2;
    _gameState = EngineState::getEngineState();

    //Used for god mode
    _currCamera = &_godCamera;
    _currCamera->getState()->setGravity(false);
    //Hook up to kinematic update for proper physics handling
    MasterClock::instance()->subscribeKinematicsRate(std::bind(&ViewEventDistributor::_updateKinematics, this, std::placeholders::_1));
}

ViewEventDistributor::~ViewEventDistributor() {
    delete _viewEvents;
}

void ViewEventDistributor::displayViewFrustum() {

    if (_currCamera == &_viewCamera) {
        //_godCamera.displayViewFrustum();
    }
    else if (_currCamera == &_godCamera) {
        _viewCamera.displayViewFrustum(_godCamera.getView());
    }
}

void ViewEventDistributor::setProjection(unsigned int viewportWidth, unsigned int viewportHeight, float nearPlaneDistance, float farPlaneDistance) {
    //45 degree angle up/down/left/right,
    //width by height aspect ratio
    //near plane from camera location
    //far plane from camera location
    _currCamera->setProjection(Matrix::cameraProjection(45.0f,
        static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight),
        nearPlaneDistance,
        farPlaneDistance));
    _godCamera.setProjection(_currCamera->getProjection());
    _viewCamera.setProjection(_currCamera->getProjection());

    
}

void ViewEventDistributor::setView(Matrix translation, Matrix rotation, Matrix scale) {
    Vector4 zero(0.f, 0.f, 0.f);
    _scale = scale;
    _rotation = rotation;
    _inverseRotation = rotation.inverse();
    _translation = translation;
    
    _currCamera->setView(_translation, _rotation, _scale);

    _godCamera.setViewMatrix(_currCamera->getView());
    _viewCamera.setViewMatrix(_currCamera->getView());
}

void ViewEventDistributor::triggerEvents() {

    _viewEvents->updateProjection(_currCamera->getProjection());
    _viewEvents->updateView(_currCamera->getView());
}

Matrix ViewEventDistributor::getProjection() {
    return _currCamera->getProjection();
}

Matrix ViewEventDistributor::getView() {
    return _currCamera->getView();
}

Matrix ViewEventDistributor::getFrustumProjection() {
    return _viewCamera.getProjection();
}

Matrix ViewEventDistributor::getFrustumView() {
    return _viewCamera.getView();
}

Camera::ViewState ViewEventDistributor::getViewState() {
    return _currCamera->getViewState();
}

void ViewEventDistributor::setEntityList(std::vector<Entity*> entityList) {
    _entityList = entityList;
}

ViewEvents* ViewEventDistributor::getEventWrapper() {
    return _viewEvents;
}

void ViewEventDistributor::_updateReleaseKeyboard(int key, int x, int y) { //Do stuff based on keyboard release update
                                                                  //If function state exists
    if (_keyboardState.find(key) != _keyboardState.end()) {
        delete _keyboardState[key];
        _keyboardState.erase(key); //erase by keyq
    }
}

void ViewEventDistributor::_updateKinematics(int milliSeconds) {
    //Do kinematic calculations
    _currCamera->updateState(milliSeconds);

    //Pass position information to model matrix
    Vector4 position = _currCamera->getState()->getLinearPosition();
    _translation = Matrix::translation(position.getx(), position.gety(), position.getz());
}

void ViewEventDistributor::_updateKeyboard(int key, int x, int y) { //Do stuff based on keyboard update

    if (_gameState.gameModeEnabled) {

        _currCamera->setViewState(key);

        if (key == GLFW_KEY_W || key == GLFW_KEY_S ||
            key == GLFW_KEY_A || key == GLFW_KEY_D ||
            key == GLFW_KEY_E || key == GLFW_KEY_C) {

            Vector4 trans;
            Vector4 force;
            const float velMagnitude = 500.0f;

            if (key == GLFW_KEY_W) { //forward w
                trans = Vector4(_inverseRotation * Vector4(0.0, 0.0, -velMagnitude));
            }
            else if (key == GLFW_KEY_S) { //backward s
                trans = Vector4(_inverseRotation * Vector4(0.0, 0.0, velMagnitude));
            }
            else if (key == GLFW_KEY_A) { //left a
                trans = Vector4(_inverseRotation * Vector4(-velMagnitude, 0.0, 0.0));
            }
            else if (key == GLFW_KEY_D) { //right d
                trans = Vector4(_inverseRotation * Vector4(velMagnitude, 0.0, 0.0));
            }
            else if (key == GLFW_KEY_E) { //up e
                trans = Vector4(_inverseRotation * Vector4(0.0, velMagnitude, 0.0));
            }
            else if (key == GLFW_KEY_C) { //down c
                trans = Vector4(_inverseRotation * Vector4(0.0, -velMagnitude, 0.0));
            }

            //If not in god camera view mode then push view changes to the model for full control of a model's movements
            if (!_godState && _entityIndex < _entityList.size()) {

                StateVector* state = _entityList[_entityIndex]->getStateVector();
                //Define lambda equation
                auto lamdaEq = [=](float t) -> Vector4 {
                    if (t > 1.0f) {
                        return trans;
                    }
                    else {
                        return static_cast<Vector4>(trans) * t;
                    }
                };
                //lambda function container that manages force model
                //Last forever in intervals of 5 milliseconds
                FunctionState* func = new FunctionState(std::bind(&StateVector::setForce, state, std::placeholders::_1),
                    lamdaEq,
                    5);

                //Keep track to kill function when key is released
                _keyboardState[key] = func;
            }
            else if (_godState) {

                _currCamera->getState()->setActive(true);

                //Define lambda equation
                auto lamdaEq = [trans](float t) -> Vector4 {
                    if (t > 1.0f) {
                        return trans;
                    }
                    else {
                        return static_cast<Vector4>(trans) * t;
                    }
                };
                //lambda function container that manages force model
                //Last forever in intervals of 5 milliseconds
                FunctionState* func = new FunctionState(std::bind(&StateVector::setForce, _currCamera->getState(), std::placeholders::_1),
                    lamdaEq,
                    5);

                //Keep track to kill function when key is released
                _keyboardState[key] = func;
            }
        }
        else if (key == GLFW_KEY_G) { //God's eye view change g
            _godState = true;
            _currCamera = &_godCamera;

            _translation = Matrix::cameraTranslation(0, 5, 0); //Reset to 0,5,0 view position
            _rotation = Matrix(); //Set rotation matrix to identity
            _inverseRotation = Matrix();
            _currCamera->setViewMatrix(_rotation * _translation);
            _viewEvents->updateView(_currCamera->getView()); //Send out event to all listeners to offset locations essentially
        }
        else if (key == GLFW_KEY_Q) { //Cycle through model's view point q

            _entityIndex++; //increment to the next model when q is pressed again
            if (_entityIndex >= _entityList.size()) {
                _entityIndex = 0;
            }

            _godState = false;
            _currCamera = &_viewCamera;

            StateVector* state = _entityList[_entityIndex]->getStateVector();
            float* position = state->getLinearPosition().getFlatBuffer();
            float* rotation = state->getAngularPosition().getFlatBuffer();
            _translation = Matrix::cameraTranslation(position[0], position[1], position[2]); //Set camera to model's world space translation
            Matrix rotY = Matrix::cameraRotationAroundY(rotation[1]); //Set rotation around Y
            Matrix rotX = Matrix::cameraRotationAroundX(rotation[0]); //Set rotation around X
            Matrix rotZ = Matrix::cameraRotationAroundZ(rotation[2]); //Set rotation around Z
            _rotation = rotZ * rotX * rotY;
            _inverseRotation = Matrix();

            //Last transform to be applied to achieve third person view
            _currCamera->setViewMatrix(_thirdPersonTranslation * _rotation * _translation); //translate then rotate around point
            _viewEvents->updateView(_currCamera->getView()); //Send out event to all listeners to offset locations essentially

        }
    }
}

void ViewEventDistributor::_updateGameState(EngineStateFlags state) {
    _gameState = state;
}

void ViewEventDistributor::_updateMouse(double x, double y) { //Do stuff based on mouse update

    static const double mouseSensitivity = 1.5f;

    //Filter out large changes because that causes view twitching
    if (x != _prevMouseX) {
    
        FunctionState* ptr = nullptr;
        //Use special value 200 to indicate mouse is moving
        if (_keyboardState.find(200) != _keyboardState.end()) {
            ptr = _keyboardState[200];
        }

        double diffX = _prevMouseX - x;
           
        Vector4 newRot;
        if (diffX > 0) { //rotate left around y axis
            newRot = Vector4(0.0, static_cast<float>(mouseSensitivity * diffX), 0.0);
        }
        else if (diffX < 0) { //rotate right around y axis
            newRot = Vector4(0.0, static_cast<float>(mouseSensitivity * diffX), 0.0);
        }

        Vector4 rot;
        if (ptr != nullptr) {
            rot = ptr->getVectorState();
        }

        //Define lambda equation
        auto lamdaEq = [rot, newRot](float t) -> Vector4 {
            if (t > 0.05f) {
                return Vector4(0.0,0.0,0.0);
                //return ((static_cast<Vector4>(newRot)/* + static_cast<Vector4>(rot)*/) * exp(-10.0f*t));
            }
            else {
                return static_cast<Vector4>(newRot) + static_cast<Vector4>(rot);
            }
        };

        //lambda function container that manages force model
        //Last forever in intervals of 5 milliseconds
        FunctionState* func = new FunctionState(std::bind(&StateVector::setTorque, _currCamera->getState(), std::placeholders::_1),
            lamdaEq,
            5);

        delete _keyboardState[200];
        _keyboardState.erase(200);
        _keyboardState[200] = func;

        //If not in god camera view mode then push view changes to the model for full control of a model's movements
        if (!_godState) {
            _currCamera->setViewMatrix(_thirdPersonTranslation * _currCamera->getView());
        }
        _currCamera->getState()->setActive(true);
    }

    if (y != _prevMouseY) {
        double diffY = _prevMouseY - y;
    }

    _prevMouseX = x;
    _prevMouseY = y;
}
void ViewEventDistributor::_updateDraw() { //Do draw stuff

    //If not in god camera view mode then push view changes to the model for full control of a model's movements
    if (!_godState && _entityIndex < _entityList.size()) {

        StateVector* state = _entityList[_entityIndex]->getStateVector();
        float* pos = state->getLinearPosition().getFlatBuffer();
        float* rot = _currCamera->getState()->getAngularPosition().getFlatBuffer();
        _translation = Matrix::cameraTranslation(pos[0], pos[1], pos[2]); //Update the translation state matrix
        _rotation = Matrix::cameraRotationAroundY(rot[1]); //Update the rotation state matrix
        _inverseRotation = Matrix::cameraRotationAroundY(-rot[1]);

        //Last transform to be applied to achieve third person view
        _currCamera->setViewMatrix(_thirdPersonTranslation * _rotation * _translation); //translate then rotate around point
        _viewEvents->updateView(_currCamera->getView()); //Send out event to all listeners to offset locations essentially

    }
    else if (_godState) {

        float* pos = _currCamera->getState()->getLinearPosition().getFlatBuffer();
        float* rot = _currCamera->getState()->getAngularPosition().getFlatBuffer();
        _translation = Matrix::cameraTranslation(pos[0], pos[1], pos[2]); //Update the translation state matrix
        _rotation = Matrix::cameraRotationAroundY(rot[1]); //Update the rotation state matrix
        _inverseRotation = Matrix::cameraRotationAroundY(-rot[1]);
        
        _currCamera->setViewMatrix(_rotation * _translation); //translate then rotate around point
        _viewEvents->updateView(_currCamera->getView()); //Send out event to all listeners to offset locations essentially
    }
}