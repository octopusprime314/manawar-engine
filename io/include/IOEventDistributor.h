/*
* IOEventDistributor is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
* Copyright (c) 2017 Peter Morley.
*
* ReBoot is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, version 3.
*
* ReBoot is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/**
*  IOEventDistributor class. Responsible for distributing input/view manager events to objects
*  that derive from the EventSubscriber class.
*/

#pragma once
#include "GLIncludes.h"
#include "ViewEventDistributor.h"
#include <functional>
#include <vector>
#include <mutex>
#include "IOEvents.h"
#include <queue>

class DXLayer;

struct TimeEvent {
    typedef void (Callback)();

    uint64_t time;
    Callback* pfnCallback;

    TimeEvent(uint64_t time, Callback* pfnCallback)
        : time(time),
        pfnCallback(pfnCallback) {}
};

static bool operator<(TimeEvent left, TimeEvent right) {
    return left.time > right.time;
}

class IOEventDistributor {


public:
    IOEventDistributor(int* argc, char** argv, HINSTANCE hInstance, int nCmdShow);

    void                                  run(); //Function used to run context
    void                                  subscribeToKeyboard(std::function<void(int, int, int)> func); //Use this call to connect functions up to key updates
    void                                  subscribeToReleaseKeyboard(std::function<void(int, int, int)> func); //Use this call to connect functions up to key release updates
    void                                  subscribeToMouse(std::function<void(double, double)> func); //Use this call to connect functions up to mouse updates
    void                                  subscribeToDraw(std::function<void()> func); //Use this call to connect functions up to draw updates
    void                                  subscribeToGameState(std::function<void(EngineStateFlags)> func); //Use this call to connect functions up to game state updates
    void                                  updateGameState(EngineStateFlags state);
    static int                            screenPixelWidth;
    static int                            screenPixelHeight;
    static LRESULT CALLBACK               dxEventLoop(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static void                           quit();

private:

    IOEvents                              _events; //Event wrapper for GLFW based events
    unsigned int                          _viewportWidth;  //Width dimension of glfw window context in pixel units
    unsigned int                          _viewportHeight; //Height dimension of glfw window context in pixel units
    static std::priority_queue<TimeEvent> _timeEvents; // Events that trigger at a specific time
    static int                            _renderNow;      //Internal flag that coordinates with the framerate
    static std::mutex                     _renderLock;     //Prevents write/write collisions with renderNow on a frame tick trigger
    static GLFWwindow*                    _window;         //Glfw window
    static bool                           _quit;           //Notifies render loop that game is over

    //All keyboard input from glfw will be notified here
    static void                           _keyboardUpdate(GLFWwindow* window, int key, int scancode, int action, int mods);
    //One frame draw update call
    static void                           _drawUpdate();
    //All mouse movement input will be notified here
    static void                           _mouseUpdate(GLFWwindow* window, double x, double y);
    //Mouse click input will be notified here
    static void                           _mouseClick(GLFWwindow* window, int button, int action, int mods);
    //Simple context synchronizes frame rate using the MasterClock tuning capability
    static void                           _frameRateTrigger(int milliSeconds);
};