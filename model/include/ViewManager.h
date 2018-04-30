/*
* ViewManager is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  ViewManager class.  Manages the transformation of the view state.
*  Geometry that live in model-world space need to be updated in opposite
*  fashion in order to properly move geometry around view.
*  Classes that derive from UpdateInterface will receive view manager
*  change events and will transform their model-world space to the updated
*  view transform.
*/

#pragma once
#include "UpdateInterface.h"
#include "Matrix.h"
#include "Vector4.h"
#include "ViewManagerEvents.h"
#include <map>

class SimpleContext;
class Model;
class FunctionState;
class ViewManager : public UpdateInterface {

    enum class ViewState{
        DIFFUSE = 0,
        LIGHT_DEPTH,
        SHADOW_MAPPING, 
        DIFFUSE_DIR_LIGHT,
        NORMAL,
        POSITION,
        DIFFUSE_SHADOW,
        POINT_LIGHTS
    };
    ViewState           _viewState;
    Matrix              _view;
    Matrix              _projection;
    Matrix              _translation; //Keep track of translation state
    Matrix              _rotation; //Keep track of rotation state
    Matrix              _scale; //Keep track of scale state
    Matrix              _thirdPersonTranslation;
    Matrix              _inverseRotation; //Manages how to translate based on the inverse of the actual rotation matrix
    ViewManagerEvents*  _viewEvents;
    SimpleContext*      _glutContext;
    std::vector<Model*> _modelList; //used to translate view to a model's transformation
    int                 _modelIndex; //used to keep track of which model the view is set to
    bool                _godState; //indicates whether the view is in god or model view point mode
    std::map<unsigned char, FunctionState*> _keyboardState; 
public:
    ViewManager();
    ViewManager(int* argc, char** argv, unsigned int viewportWidth, unsigned int viewportHeight);
    ~ViewManager();
    void               applyTransform(Matrix transform);
    void               setProjection(unsigned int viewportWidth, unsigned int viewportHeight, float nearPlaneDistance, float farPlaneDistance);
    void               setView(Matrix translation, Matrix rotation, Matrix scale);
    void               setModelList(std::vector<Model*> modelList);
    Matrix&            getProjection();
    Matrix&            getView();
    ViewState          getViewState();
    ViewManagerEvents* getEventWrapper();
    void               run(); //Make this call to start glut mainloop
protected:
    void               _updateKeyboard(int key, int x, int y); //Do stuff based on keyboard upate
	void               _updateReleaseKeyboard(int key, int x, int y); //Do stuff based on keyboard release upate
    void               _updateMouse(double x, double y); //Do stuff based on mouse update
    void               _updateDraw(); //Do draw stuff
};