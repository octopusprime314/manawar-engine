#pragma once
#include "GLIncludes.h"
#include "ViewManager.h"
#include <functional>
#include <vector>
#include "SimpleContextEvents.h"

class SimpleContext{

public:
	SimpleContext(int* argc, char** argv);

	void run(); //Function used to run context as a thread managed elsewhere
	
	void subscribeToKeyboard(std::function<void(unsigned char, int, int)> func); //Use this call to connect functions up to key updates
	void subscribeToMouse(std::function<void(int, int, int, int)> func); //Use this call to connect functions up to mouse updates
	void subscribeToDraw(std::function<void()> func); //Use this call to connect functions up to draw updates
	
private:

	SimpleContextEvents _events; //Event wrapper for GLUT based events

	//All keyboard input from glut will be notified here
	static void _keyboardUpdate(unsigned char key, int x, int y);

	//One frame draw update call
	static void _drawUpdate();

	//All mouse press input from glut will be notified here
	static void _mouseUpdate(int button, int state, int x, int y);
	//All mouse movement input from glut will be notified here
	static void _mouseUpdate(int x, int y);
};