#pragma once
#include "GLIncludes.h"
#include "ViewManager.h"
#include <functional>
#include <vector>

class SimpleContext{

public:
	SimpleContext(int* argc, char** argv);

	static void subscribeToKeyboard(std::function<void(unsigned char, int, int)> func); //Use this call to connect functions up to key updates
	static void subscribeToMouse(std::function<void(int, int, int, int)> func); //Use this call to connect functions up to mouse updates
	static void subscribeToDraw(std::function<void()> func); //Use this call to connect functions up to draw updates
	static void subscribeToView(std::function<void(Matrix)> func); //Use this call to connect functions up to view/camera updates
	static void broadcastViewMatrix(Matrix view); //Broadcasts out view matrix update to all subscribers
	static void run(); //Function used to run context as a thread managed elsewhere

private:

	static std::vector<std::function<void(unsigned char, int, int)>> _keyboardFuncs;
	static std::vector<std::function<void(int, int, int, int)>> _mouseFuncs;
	static std::vector<std::function<void()>> _drawFuncs;
	static std::vector<std::function<void(Matrix)>> _viewFuncs;

	ViewManager _viewManager; //manages the view/camera matrix from the user's perspective
	
	//All keyboard input from glut will be notified here
	static void _keyboardUpdate(unsigned char key, int x, int y);

	//One frame draw update call
	static void _drawUpdate();

	//All mouse press input from glut will be notified here
	static void _mouseUpdate(int button, int state, int x, int y);
	//All mouse movement input from glut will be notified here
	static void _mouseUpdate(int x, int y);
};