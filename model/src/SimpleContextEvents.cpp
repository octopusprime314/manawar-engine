#include "SimpleContextEvents.h"
#include "GLIncludes.h"

std::vector<std::function<void(unsigned char, int, int)>> SimpleContextEvents::_keyboardFuncs;
std::vector<std::function<void(unsigned char, int, int)>> SimpleContextEvents::_keyboardReleaseFuncs;
std::vector<std::function<void(int, int, int, int)>> SimpleContextEvents::_mouseFuncs;
std::vector<std::function<void()>> SimpleContextEvents::_drawFuncs;
std::function<void()> SimpleContextEvents::_preDrawCallback;
std::function<void()> SimpleContextEvents::_postDrawCallback;

void SimpleContextEvents::subscribeToKeyboard(std::function<void(unsigned char, int, int)> func) { //Use this call to connect functions to key updates
    _keyboardFuncs.push_back(func);
}
void SimpleContextEvents::subscribeToReleaseKeyboard(std::function<void(unsigned char, int, int)> func) { //Use this call to connect functions to key updates
    _keyboardReleaseFuncs.push_back(func);
}
void SimpleContextEvents::subscribeToMouse(std::function<void(int, int, int, int)> func) { //Use this call to connect functions to mouse updates
    _mouseFuncs.push_back(func);
}
void SimpleContextEvents::subscribeToDraw(std::function<void()> func) { //Use this call to connect functions to draw updates
    _drawFuncs.push_back(func);
}

void SimpleContextEvents::setPreDrawCallback(std::function<void()> func) {
    _preDrawCallback = func;
}
void SimpleContextEvents::setPostDrawCallback(std::function<void()> func) {
    _postDrawCallback = func;
}

//All keyboard input from glut will be notified here
void SimpleContextEvents::updateKeyboard(unsigned char key, int x, int y) {

    if (key == 27) //Escape key pressed, hard exit no cleanup, TODO FIX THIS!!!!
        exit(0);

    for (auto func : _keyboardFuncs) {
        func(key, x, y); //Call keyboard update
    }
}

//All keyboard input from glut will be notified here
void SimpleContextEvents::releaseKeyboard(unsigned char key, int x, int y) {

    for (auto func : _keyboardReleaseFuncs) {
        func(key, x, y); //Call keyboard release update
    }
}

//One frame draw update call
void SimpleContextEvents::updateDraw() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Call scene manager to go any global operations before drawing
    _preDrawCallback();

    for (auto func : _drawFuncs) {
        func(); //Call draw update method
    }

    //Call scene manager to go any global operations after drawing
    _postDrawCallback();

    glutSwapBuffers(); // Double buffering
}

//All mouse input presses from glut will be notified here
void SimpleContextEvents::updateMouse(int button, int state, int x, int y) {
    for (auto func : _mouseFuncs) {
        func(button, state, x, y); //Call mouse click update
    }
}

//All passive mouse movement input from glut will be notified here
void SimpleContextEvents::updateMouse(int x, int y) {
    for (auto func : _mouseFuncs) {
        func(0, 0, x, y); //Call mouse movement update 
    }
}