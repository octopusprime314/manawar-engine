#pragma once
#include <vector>
#include <functional>
#include "Matrix.h"

class SimpleContextEvents{

	static std::vector<std::function<void(unsigned char, int, int)>> _keyboardFuncs;
	static std::vector<std::function<void(int, int, int, int)>> _mouseFuncs;
	static std::vector<std::function<void()>> _drawFuncs;

public:
	static void subscribeToKeyboard(std::function<void(unsigned char, int, int)> func); //Use this call to connect functions up to key updates
	static void subscribeToMouse(std::function<void(int, int, int, int)> func); //Use this call to connect functions up to mouse updates
	static void subscribeToDraw(std::function<void()> func); //Use this call to connect functions up to draw updates

	static void updateKeyboard(unsigned char key, int x, int y);
	static void updateDraw();
	static void updateMouse(int button, int state, int x, int y);
	static void updateMouse(int x, int y);
};