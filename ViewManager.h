#pragma once
#include "UpdateInterface.h"
#include "Matrix.h"
#include "Vector4.h"
class ViewManager : public UpdateInterface{

    Vector4 _pos;
    Matrix _view;
    Matrix _projection;

public:

    ViewManager() {}
    void applyTransform(Matrix transform);
	void updateSubscribers();

protected:
	void updateKeyboard(unsigned char key, int x, int y); //Do stuff based on keyboard upate
	void updateMouse(int button, int state, int x, int y); //Do stuff based on mouse update
	void updateDraw(); //Do draw stuff
};