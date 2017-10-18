#pragma once
#include "UpdateInterface.h"
#include "Matrix.h"
#include "Vector4.h"
class ViewManager : public UpdateInterface{

    Matrix _view;
    Matrix _projection;

	Matrix _translation; //Keep track of translation state
	Matrix _rotation; //Keep track of rotation state

	Matrix _inverseRotation; //Manages how to translate based on the inverse of the actual rotation matrix

public:

    ViewManager();
    void applyTransform(Matrix transform);
	void setProjection();

protected:
	void updateKeyboard(unsigned char key, int x, int y); //Do stuff based on keyboard upate
	void updateMouse(int button, int state, int x, int y); //Do stuff based on mouse update
	void updateDraw(); //Do draw stuff
};