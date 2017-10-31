#pragma once
#include "GLIncludes.h"
#include "ViewManager.h"
#include <functional>
#include <vector>
#include "SimpleContextEvents.h"

class SimpleContext{

public:
	SimpleContext(int* argc, char** argv, unsigned int viewportWidth, unsigned int viewportHeight);

	void run(); //Function used to run context
	
	void subscribeToKeyboard(std::function<void(unsigned char, int, int)> func); //Use this call to connect functions up to key updates
	void subscribeToMouse(std::function<void(int, int, int, int)> func); //Use this call to connect functions up to mouse updates
	void subscribeToDraw(std::function<void()> func); //Use this call to connect functions up to draw updates
	
private:

	SimpleContextEvents _events; //Event wrapper for GLUT based events
	unsigned int viewportWidth;  //Width dimension of glut window context in pixel units
	unsigned int viewportHeight; //Height dimension of glut window context in pixel units

	//All keyboard input from glut will be notified here
	static void _keyboardUpdate(unsigned char key, int x, int y);

	//One frame draw update call
	static void _drawUpdate();

	//All mouse press input from glut will be notified here
	static void _mouseUpdate(int button, int state, int x, int y);
	//All mouse movement input from glut will be notified here
	static void _mouseUpdate(int x, int y);
};