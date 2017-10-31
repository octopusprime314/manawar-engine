#pragma once
#include "UpdateInterface.h"
#include "Matrix.h"
#include "Vector4.h"
#include "ViewManagerEvents.h"

class SimpleContext;
class ViewManager : public UpdateInterface{

    Matrix _view;
    Matrix _projection;

	Matrix _translation; //Keep track of translation state
	Matrix _rotation; //Keep track of rotation state

	Matrix _inverseRotation; //Manages how to translate based on the inverse of the actual rotation matrix

	ViewManagerEvents* _viewEvents;

	SimpleContext* _glutContext;
public:

    ViewManager();
	ViewManager(int* argc, char** argv, unsigned int viewportWidth, unsigned int viewportHeight);
    void applyTransform(Matrix transform);
	void setProjection(unsigned int viewportWidth, unsigned int viewportHeight, float nearPlaneDistance, float farPlaneDistance);
	ViewManagerEvents* getEventWrapper();
	void run(); //Make this call to start glut mainloop

protected:
	void updateKeyboard(unsigned char key, int x, int y); //Do stuff based on keyboard upate
	void updateMouse(int button, int state, int x, int y); //Do stuff based on mouse update
	void updateDraw(); //Do draw stuff
};