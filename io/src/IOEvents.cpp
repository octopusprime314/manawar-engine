#include "IOEvents.h"
#include "GLIncludes.h"
#include "EngineManager.h"
#include "ShaderBroker.h"

std::vector<std::function<void(int, int, int)>>      IOEvents::_keyboardReleaseFuncs;
std::function<void()>                                IOEvents::_postDrawCallback;
std::vector<std::function<void(int, int, int, int)>> IOEvents::_mouseButtonFuncs;
std::function<void()>                                IOEvents::_preDrawCallback;
std::vector<std::function<void(EngineStateFlags)>>   IOEvents::_gameStateFuncs;
std::vector<std::function<void(int, int, int)>>      IOEvents::_keyboardFuncs;
std::vector<std::function<void(double, double)>>     IOEvents::_mouseFuncs;
std::vector<std::function<void()>>                   IOEvents::_drawFuncs;

//Use this call to connect functions to key updates
void IOEvents::subscribeToKeyboard(std::function<void(int, int, int)> func) { 
    _keyboardFuncs.push_back(func);
}
//Use this call to connect functions to key updates
void IOEvents::subscribeToReleaseKeyboard(std::function<void(int, int, int)> func) { 
    _keyboardReleaseFuncs.push_back(func);
}
//Use this call to connect functions to mouse updates
void IOEvents::subscribeToMouse(std::function<void(double, double)> func) { 
    _mouseFuncs.push_back(func);
}
//Use this call to connect functions to draw updates
void IOEvents::subscribeToDraw(std::function<void()> func) { 
    _drawFuncs.push_back(func);
}
//Use this call to connect functions to key updates
void IOEvents::subscribeToGameState(std::function<void(EngineStateFlags)> func) { 
    _gameStateFuncs.push_back(func);
}
//Use this call to connect functions to mouse button updates
void IOEvents::subscribeToMouseClick(std::function<void(int, int, int, int)> func) { 
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
        if (func != nullptr) {
            func(key, x, y);
        }
    }
}

//All keyboard input from glfw will be notified here
void IOEvents::releaseKeyboard(int key, int x, int y) {

    for (auto func : _keyboardReleaseFuncs) {
        if (func != nullptr) {
            func(key, x, y);
        }
    }
}

//One frame draw update call
void IOEvents::updateDraw(GLFWwindow* _window) {

    //Call scene manager to go any global operations before drawing
    _preDrawCallback();
    
    for (auto func : _drawFuncs) {
        if (func != nullptr) {
            func();
        }
    }

    //Call scene manager to go any global operations after drawing
    _postDrawCallback();

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        glfwSwapBuffers(_window);

        //Poll for events
        glfwPollEvents();
    }
}


//All mouse movement input will be notified here
void IOEvents::updateMouse(double x, double y) {
    for (auto func : _mouseFuncs) {
        if (func != nullptr) {
            func(x, y);
        }
    }
}

//All mouse button input will be notified here
void IOEvents::updateMouseClick(int button, int action, int x, int y) {

    for (auto func : _mouseButtonFuncs) {
        if (func != nullptr) {
            func(button, action, x, y);
        }
    }
}

void IOEvents::updateGameState(EngineStateFlags state) {
    //first update static game state
    EngineState::setEngineState(state);
    for (auto func : _gameStateFuncs) {
        if (func != nullptr) {
            func(state);
        }
    }
}
