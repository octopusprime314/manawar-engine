#pragma once
#include "GLIncludes.h"
#include <functional>
#include <vector>

class SimpleContext{

public:
	SimpleContext(int* argc, char** argv);

	static void subscribeToKeyboard(std::function<void(unsigned char, int, int)> func); //Use this call to connect functions up to key updates
	static void subscribeToMouse(std::function<void(int, int, int, int)> func); //Use this call to connect functions up to mouse updates
	static void run(); //Function used to run context as a thread managed elsewhere

private:

	static std::vector<std::function<void(unsigned char, int, int)>> _keyboardFuncs;
	static std::vector<std::function<void(int, int, int, int)>> _mouseFuncs;
	
	//All keyboard input from glut will be notified here
	static void _keyboardUpdate(unsigned char key, int x, int y);

	//One frame draw update call
	static void _drawUpdate();

	//All mouse press input from glut will be notified here
	static void _mouseUpdate(int button, int state, int x, int y);
	//All mouse movement input from glut will be notified here
	static void _mouseUpdate(int x, int y);
};