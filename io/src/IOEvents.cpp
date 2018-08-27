#include "IOEvents.h"
#include "GLIncludes.h"

std::vector<std::function<void(int, int, int)>> IOEvents::_keyboardFuncs;
std::vector<std::function<void(int, int, int)>> IOEvents::_keyboardReleaseFuncs;
std::vector<std::function<void(int, int, int, int)>> IOEvents::_mouseButtonFuncs;
std::vector<std::function<void(double, double)>> IOEvents::_mouseFuncs;
std::vector<std::function<void()>> IOEvents::_drawFuncs;
std::vector<std::function<void(EngineStateFlags)>> IOEvents::_gameStateFuncs;
std::function<void()> IOEvents::_preDrawCallback;
std::function<void()> IOEvents::_postDrawCallback;

void IOEvents::subscribeToKeyboard(std::function<void(int, int, int)> func) { //Use this call to connect functions to key updates
    _keyboardFuncs.push_back(func);
}
void IOEvents::subscribeToReleaseKeyboard(std::function<void(int, int, int)> func) { //Use this call to connect functions to key updates
    _keyboardReleaseFuncs.push_back(func);
}
void IOEvents::subscribeToMouse(std::function<void(double, double)> func) { //Use this call to connect functions to mouse updates
    _mouseFuncs.push_back(func);
}
void IOEvents::subscribeToDraw(std::function<void()> func) { //Use this call to connect functions to draw updates
    _drawFuncs.push_back(func);
}
void IOEvents::subscribeToGameState(std::function<void(EngineStateFlags)> func) { //Use this call to connect functions to key updates
    _gameStateFuncs.push_back(func);
}
void IOEvents::subscribeToMouseClick(std::function<void(int, int, int, int)> func) { //Use this call to connect functions to mouse button updates
    _mouseButtonFuncs.push_back(func);
}

void IOEvents::setPreDrawCallback(std::function<void()> func) {
    _preDrawCallback = func;
}
void IOEvents::setPostDrawCallback(std::function<void()> func) {
    _postDrawCallback = func;
}

//All keyboard input from glfw will be notified here
void IOEvents::updateKeyboard(int key, int x, int y) {

    for (auto func : _keyboardFuncs) {
        func(key, x, y); //Call keyboard update
    }
}

//All keyboard input from glfw will be notified here
void IOEvents::releaseKeyboard(int key, int x, int y) {

    for (auto func : _keyboardReleaseFuncs) {
        func(key, x, y); //Call keyboard release update
    }
}

//One frame draw update call
void IOEvents::updateDraw(GLFWwindow* _window) {

    //Call scene manager to go any global operations before drawing
    _preDrawCallback();

    for (auto func : _drawFuncs) {
        func(); //Call draw update method
    }

    //Call scene manager to go any global operations after drawing
    _postDrawCallback();

    glfwSwapBuffers(_window); // Double buffering

    glfwPollEvents(); //Poll for events
}


//All mouse movement input will be notified here
void IOEvents::updateMouse(double x, double y) {
    for (auto func : _mouseFuncs) {
        func(x, y); //Call mouse movement update
    }
}

//All mouse button input will be notified here
void IOEvents::updateMouseClick(int button, int action, int x, int y) {

    for (auto func : _mouseButtonFuncs) {
        func(button, action, x, y); //Call mouse movement update
    }
}

void IOEvents::updateGameState(EngineStateFlags state) {
    //first update static game state
    EngineState::setEngineState(state);
    for (auto func : _gameStateFuncs) {
        func(state); //Call game state update
    }
}
