/*
* ViewEventDistributor is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
* Copyright (c) 2017 Peter Morley.
*
* ReBoot is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, version 3.
*
* ReBoot is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/**
*  ViewEventDistributor class.  Manages the transformation of the view state.
*  Geometry that live in model-world space need to be updated in opposite
*  fashion in order to properly move geometry around view.
*  Classes that derive from EventSubscriber will receive view manager
*  change events and will transform their model-world space to the updated
*  view transform.
*/

#pragma once
#include "Camera.h"
#include "WaypointCamera.h"
#include "VectorCamera.h"
#include "ViewEvents.h"
#include "EventSubscriber.h"
#include "IOEventDistributor.h"
#include <memory>

class Model;
class Entity;
class FunctionState;
using FuncMap = std::map<unsigned char, FunctionState*>;
class ViewEventDistributor : public EventSubscriber {
public:
    enum class ViewState {
        DEFERRED_LIGHTING = 0,
        DIFFUSE,
        NORMAL,
        POSITION,
        VELOCITY,
        SCREEN_SPACE_AMBIENT_OCCLUSION,
        CAMERA_SHADOW,
        MAP_SHADOW,
        POINT_SHADOW,
        PHYSICS
    };

private:
    Matrix               _translation; //Keep track of translation state
    Matrix               _rotation; //Keep track of rotation state
    Matrix               _scale; //Keep track of scale state
    Matrix               _thirdPersonTranslation;
    Matrix               _inverseRotation; //Manages how to translate based on the inverse of the actual rotation matrix
    ViewEvents*          _viewEvents;
    std::vector<Entity*> _entityList; //used to translate view to a model's transformation
    int                  _entityIndex; //used to keep track of which model the view is set to
    bool                 _godState; //indicates whether the view is in god or model view point mode
    bool                 _trackedState; // indicates whether the camera is on a track
    FuncMap              _keyboardState;
    double               _prevMouseX, _prevMouseY;
    double               _currMouseX, _currMouseY;
    void                 _updateKinematics(int milliSeconds);
    Camera               _viewCamera;
    Camera               _godCamera;
    WaypointCamera       _waypointCamera;
    VectorCamera         _vectorCamera;
    Camera*              _trackedCamera;
    Camera*              _currCamera;
    EngineStateFlags     _gameState;

public:
    ViewEventDistributor();
    ViewEventDistributor(int* argc, char** argv, unsigned int viewportWidth, unsigned int viewportHeight);
    ~ViewEventDistributor();
    void                 setProjection(unsigned int viewportWidth, 
                                       unsigned int viewportHeight, 
                                       float nearPlaneDistance, 
                                       float farPlaneDistance);
    void                 setView(Matrix translation, Matrix rotation, Matrix scale);
    void                 setEntityList(std::vector<Entity*> entityList);
    Matrix               getProjection();
    Matrix               getView();
    Matrix               getFrustumProjection();
    Matrix               getFrustumView();
    ViewEvents*          getEventWrapper();
    Camera::ViewState    getViewState();
    void                 displayViewFrustum();
    void                 triggerEvents();
    Vector4              getCameraPos();
protected:
    void                 _updateKeyboard(int key, int x, int y); //Do stuff based on keyboard upate
    void                 _updateReleaseKeyboard(int key, int x, int y); //Do stuff based on keyboard release upate
    void                 _updateMouse(double x, double y); //Do stuff based on mouse update
    void                 _updateGameState(EngineStateFlags state);
    void                 _updateDraw(); //Do draw stuff
};
