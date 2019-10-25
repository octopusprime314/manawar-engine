#define _USE_MATH_DEFINES
#include "ViewEventDistributor.h"
#include "Entity.h"
#include "FunctionState.h"
#include "Logger.h"
#include "Matrix.h"
#include "Model.h"
#include "ShaderBroker.h"
#include "StateVector.h"
#include "ViewEvents.h"
#include <cmath>
#include <iostream>

ViewEventDistributor::ViewEventDistributor() { _viewEvents = new ViewEvents(); }

ViewEventDistributor::ViewEventDistributor(int*         argc,
                                           char**       argv,
                                           unsigned int viewportWidth,
                                           unsigned int viewportHeight) {

    _viewEvents   = new ViewEvents();
    _godState     = true;  // Start in god view mode
    _trackedState = false; // Don't start on the track...yet
    _entityIndex  = 0;     // Start at index 0

    _thirdPersonTranslation = Matrix::translation(0, -5, -10);

    _prevMouseX = IOEventDistributor::screenPixelWidth / 2;
    _prevMouseY = IOEventDistributor::screenPixelHeight / 2;
    _gameState  = EngineState::getEngineState();

    // Build tracked camera
    std::vector<CameraWaypoint> waypoints;
    waypoints.emplace_back(CameraWaypoint(Vector4(0, -40.0f, 450.0f), Vector4(0, 0, 0)));
    _waypointCamera.setWaypoints(waypoints);

    std::string vec_file = "../assets/path.txt";
    _vectorCamera.setVectorsFromFile(vec_file);
    _trackedCamera = &_vectorCamera;

    //// Used to enable tracked camera
    //_trackedState = true;
    //_godState     = false;
    //_currCamera = _trackedCamera;
    _currCamera = &_godCamera;
    _waypointCamera.reset();
    _vectorCamera.reset();
    _currCamera->getState()->setGravity(false);
    _currCamera->getState()->setActive(true);

    // Hook up to kinematic update for proper physics handling
    MasterClock::instance()->subscribeKinematicsRate(
        std::bind(&ViewEventDistributor::_updateKinematics, this, std::placeholders::_1));
}

ViewEventDistributor::~ViewEventDistributor() { delete _viewEvents; }

void ViewEventDistributor::displayViewFrustum() {

    if (_currCamera == &_viewCamera) {
        //_godCamera.displayViewFrustum();
    } else if (_currCamera == &_godCamera) {
        _viewCamera.displayViewFrustum(_godCamera.getView());
    } else if (_currCamera == _trackedCamera) {
        _viewCamera.displayViewFrustum(_trackedCamera->getView());
    }
}
Vector4 ViewEventDistributor::getCameraPos() {
    Vector4 position = _currCamera->getState()->getLinearPosition();
    return Vector4(-position.getx(), -position.gety(), -position.getz());
}

void ViewEventDistributor::setProjection(unsigned int viewportWidth,
                                         unsigned int viewportHeight,
                                         float        nearPlaneDistance,
                                         float        farPlaneDistance) {
    // 45 degree angle up/down/left/right,
    // width by height aspect ratio
    // near plane from camera location
    // far plane from camera location
    _currCamera->setProjection(
        Matrix::projection(45.0f,
                           static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight),
                           nearPlaneDistance,
                           farPlaneDistance));
    _godCamera.setProjection(_currCamera->getProjection());
    _viewCamera.setProjection(_currCamera->getProjection());
    _trackedCamera->setProjection(_currCamera->getProjection());
}

void ViewEventDistributor::setView(Matrix translation, Matrix rotation, Matrix scale) {
    Vector4 zero(0.f, 0.f, 0.f);
    _scale           = scale;
    _rotation        = rotation;
    _inverseRotation = rotation.inverse();
    _translation     = translation;

    _currCamera->setView(_translation, _rotation, _scale);

    _godCamera.setViewMatrix(_currCamera->getView());
    _viewCamera.setViewMatrix(_currCamera->getView());
    _trackedCamera->setViewMatrix(_currCamera->getView());
}

void ViewEventDistributor::triggerEvents() {

    _viewEvents->updateProjection(_currCamera->getProjection());
    _viewEvents->updateView(_currCamera->getView());
}

Matrix ViewEventDistributor::getProjection() { return _currCamera->getProjection(); }

Matrix ViewEventDistributor::getView() { return _currCamera->getView(); }

Matrix ViewEventDistributor::getFrustumProjection() { return _viewCamera.getProjection(); }

Matrix ViewEventDistributor::getFrustumView() { return _viewCamera.getView(); }

Camera::ViewState ViewEventDistributor::getViewState() { return _currCamera->getViewState(); }

void ViewEventDistributor::setEntityList(std::vector<Entity*> entityList) { _entityList = entityList; }

ViewEvents* ViewEventDistributor::getEventWrapper() { return _viewEvents; }

void ViewEventDistributor::_updateReleaseKeyboard(int key, int x, int y) {
    // If function state exists
    if (_keyboardState.find(key) != _keyboardState.end()) {
        _keyboardState[key]->kill();
        delete _keyboardState[key];
        _keyboardState.erase(key); // erase by key
    }
}

void ViewEventDistributor::_updateKinematics(int milliSeconds) {

    _waypointCamera.setInversion(_inverseRotation);
    _vectorCamera.setInversion(_inverseRotation);
    Vector4 position = _currCamera->getState()->getLinearPosition();

    LOG_TRACE(-position.getx(), -position.gety(), -position.getz());
    LOG_TRACE("\n");
    // Do kinematic calculations
    _currCamera->updateState(milliSeconds);

    // Pass position information to model matrix
    _translation = Matrix::translation(-position.getx(), -position.gety(), -position.getz());
}

void ViewEventDistributor::_updateKeyboard(int key, int x,
                                           int y) { // Do stuff based on keyboard update

    if (_gameState.gameModeEnabled) {

        _currCamera->setViewState(key);

        if (!_trackedState && (key == GLFW_KEY_W || key == GLFW_KEY_S || key == GLFW_KEY_A || key == GLFW_KEY_D ||
                               key == GLFW_KEY_E || key == GLFW_KEY_C)) {

            Vector4     trans;
            const float velMagnitude = 2000.0f;

            if (key == GLFW_KEY_W) { // forward w
                trans = Vector4(_inverseRotation * Vector4(0.0, 0.0, velMagnitude));
            } else if (key == GLFW_KEY_S) { // backward s
                trans = Vector4(_inverseRotation * Vector4(0.0, 0.0, -velMagnitude));
            } else if (key == GLFW_KEY_D) { // right d
                trans = Vector4(_inverseRotation * Vector4(velMagnitude, 0.0, 0.0));
            } else if (key == GLFW_KEY_A) { // left a
                trans = Vector4(_inverseRotation * Vector4(-velMagnitude, 0.0, 0.0));
            } else if (key == GLFW_KEY_E) { // up e
                trans = Vector4(_inverseRotation * Vector4(0.0, velMagnitude, 0.0));
            } else if (key == GLFW_KEY_C) { // down c
                trans = Vector4(_inverseRotation * Vector4(0.0, -velMagnitude, 0.0));
            }

            StateVector* state = nullptr;
            // If not in god camera view mode then push view changes to the model
            // for full control of a model's movements
            if (!_godState && _entityIndex < _entityList.size()) {
                state = _entityList[_entityIndex]->getStateVector();
            } else if (_godState) {
                state = _currCamera->getState();
                state->setActive(true);
            }

            // Define lambda equation
            auto lamdaEq = [=](float t) -> Vector4 {
                if (t > 1.0f) {
                    return trans;
                } else {
                    return static_cast<Vector4>(trans) * t;
                }
            };
            // lambda function container that manages force model
            // Last forever in intervals of 5 milliseconds
            FunctionState* func =
                new FunctionState(std::bind(&StateVector::setForce, state, std::placeholders::_1), lamdaEq, 5);

            // Keep track to kill function when key is released
            if (_keyboardState.find(key) != _keyboardState.end()) {
                _keyboardState[key]->kill();
                delete _keyboardState[key];
                _keyboardState.erase(key); // erase by key
            }
            _keyboardState[key] = func;

        } else if (key == GLFW_KEY_T) {
            static const double mouseSensitivity = 15.5f;
            Vector4             newRot           = Vector4(0.0, -static_cast<float>(mouseSensitivity * 200.0f), 0.0);
            _currCamera->getState()->setTorque(newRot);

            // If not in god camera view mode then push view changes to the model for full control
            // of a model's movements
            if (!_godState) {
                _currCamera->setViewMatrix(_thirdPersonTranslation * _currCamera->getView());
            }
            _currCamera->getState()->setActive(true);
        } else if (key == GLFW_KEY_Y) {
            static const double mouseSensitivity = 15.5f;
            Vector4             newRot           = Vector4(0.0, static_cast<float>(mouseSensitivity * 200.0f), 0.0);
            _currCamera->getState()->setTorque(newRot);

            // If not in god camera view mode then push view changes to the model for full control
            // of a model's movements
            if (!_godState) {
                _currCamera->setViewMatrix(_thirdPersonTranslation * _currCamera->getView());
            }
            _currCamera->getState()->setActive(true);
        } else if (key == GLFW_KEY_G) { // God's eye view change g
            _godState     = true;
            _trackedState = false;
            _currCamera   = &_godCamera;

            StateVector* state = _currCamera->getState();
            _updateView(_currCamera, Vector4(0, -5, 0), Vector4(0, 0, 0));
        } else if (key == GLFW_KEY_T) {
            std::string vec_file = "../assets/path.txt";
            _vectorCamera.setVectorsFromFile(vec_file);
            _trackedState = true;
            _godState     = false;
            _waypointCamera.reset();
            _vectorCamera.reset();
            _currCamera = _trackedCamera;
            _currCamera->getState()->setGravity(false);
            _currCamera->getState()->setActive(true);

            setProjection(IOEventDistributor::screenPixelWidth, IOEventDistributor::screenPixelHeight, 0.1f, 5000.0f);
            setView(Matrix::translation(584.0f, -5.0f, 20.0f), Matrix::rotationAroundY(-180.0f), Matrix());

            StateVector* state = _currCamera->getState();
            _updateView(_currCamera, state->getLinearPosition(), state->getAngularPosition());
        } else if (key == GLFW_KEY_Q) { // Cycle through model's view point q

            _entityIndex++; // increment to the next model when q is pressed again
            if (_entityIndex >= _entityList.size()) {
                _entityIndex = 0;
            }

            _trackedState = false;
            _godState     = false;
            _currCamera   = &_viewCamera;

            StateVector* state = _entityList[_entityIndex]->getStateVector();
            _updateView(_currCamera, state->getLinearPosition(), state->getAngularPosition());
        }
    }
}

void ViewEventDistributor::_updateGameState(EngineStateFlags state) { _gameState = state; }

void ViewEventDistributor::_updateMouse(double x, double y) { // Do stuff based on mouse update

    static const double mouseSensitivity = 15.5f;

    if (!_trackedState && _gameState.gameModeEnabled) {

        Vector4 newRot = Vector4(0.0, 0.0, 0.0);

        // Filter out large changes because that causes view twitching
        if (x != _prevMouseX && x != IOEventDistributor::screenPixelWidth / 2) {

            double diffX = _prevMouseX - x;

            if (diffX > 0) { // rotate left around y axis
                newRot = newRot + Vector4(0.0, -static_cast<float>(mouseSensitivity * diffX), 0.0);
            } else if (diffX < 0) { // rotate right around y axis
                newRot = newRot + Vector4(0.0, -static_cast<float>(mouseSensitivity * diffX), 0.0);
            }
            _currCamera->getState()->setTorque(newRot);

            // If not in god camera view mode then push view changes to the model for full control
            // of a model's movements
            if (!_godState) {
                _currCamera->setViewMatrix(_thirdPersonTranslation * _currCamera->getView());
            }
            _currCamera->getState()->setActive(true);

            /*char str[256];
            sprintf(str, "Mouse x: %f y: %f, Delta x: %f, Delta y: %f\n", x, y, _prevMouseX - x,
            _prevMouseY - y); OutputDebugString(str);*/
        }

        if (y != _prevMouseY && y != IOEventDistributor::screenPixelHeight / 2) {

            double diffY = _prevMouseY - y;

            if (diffY > 0) { // rotate left around y axis
                newRot = newRot + Vector4(-static_cast<float>(mouseSensitivity * diffY), 0.0, 0.0);
            } else if (diffY < 0) { // rotate right around y axis
                newRot = newRot + Vector4(-static_cast<float>(mouseSensitivity * diffY), 0.0, 0.0);
            }
            _currCamera->getState()->setTorque(newRot);

            // If not in god camera view mode then push view changes to the model for full control
            // of a model's movements
            if (!_godState) {
                _currCamera->setViewMatrix(_thirdPersonTranslation * _currCamera->getView());
            }
            _currCamera->getState()->setActive(true);

            /*char str[256];
            sprintf(str, "Mouse x: %f y: %f, Delta x: %f, Delta y: %f\n", x, y, _prevMouseX - x,
            _prevMouseY - y); OutputDebugString(str);*/
        }
    }
    _prevMouseX = x;
    _prevMouseY = y;
}

void ViewEventDistributor::_updateView(Camera* camera, Vector4 posV, Vector4 rotV) {

    float* pos = posV.getFlatBuffer();
    float* rot = rotV.getFlatBuffer();
    // Update the translation state matrix
    _translation = Matrix::translation(-pos[0], -pos[1], -pos[2]);
    // Update the rotation state matrix
    _rotation        = Matrix::rotationAroundX(rot[0]) * Matrix::rotationAroundY(rot[1]);
    _inverseRotation = Matrix::rotationAroundY(-rot[1]);

    // translate then rotate around point
    camera->setViewMatrix(_rotation * _translation);
    // Send out event to all listeners to offset locations essentially
    _viewEvents->updateView(camera->getView());
}

void ViewEventDistributor::_updateDraw() { // Do draw stuff

    // If not in god camera view mode then push view changes to the model for full control of a
    // model's movements
    if (!_trackedState && !_godState && _entityIndex < _entityList.size()) {

        StateVector* state = _entityList[_entityIndex]->getStateVector();
        _updateView(_currCamera, state->getLinearPosition(), _currCamera->getState()->getAngularPosition());
    } else if (_godState || _trackedState) {

        StateVector* state = _currCamera->getState();
        _updateView(_currCamera, state->getLinearPosition(), state->getAngularPosition());
    }

    // Turn off camera rotation effects if there hasn't been a change in mouse
    if (_currMouseX == _prevMouseX && _currMouseY == _prevMouseY) {

        _currCamera->getState()->setTorque(Vector4(0.0, 0.0, 0.0));
    }
    _currMouseX = _prevMouseX;
    _currMouseY = _prevMouseY;
}