/*
* SimpleContext is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  SimpleContext class. Responsible for distributing input/view manager events to objects
*  that derive from the UpdateInterface class.
*/

#pragma once
#include "GLIncludes.h"
#include "ViewManager.h"
#include <functional>
#include <vector>
#include <mutex>
#include "SimpleContextEvents.h"

class SimpleContext {

public:
    SimpleContext(int* argc, char** argv, unsigned int viewportWidth, unsigned int viewportHeight);

    void                run(); //Function used to run context

    void                subscribeToKeyboard(std::function<void(unsigned char, int, int)> func); //Use this call to connect functions up to key updates
    void                subscribeToMouse(std::function<void(int, int, int, int)> func); //Use this call to connect functions up to mouse updates
    void                subscribeToDraw(std::function<void()> func); //Use this call to connect functions up to draw updates

private:

    SimpleContextEvents _events; //Event wrapper for GLUT based events
    unsigned int        _viewportWidth;  //Width dimension of glut window context in pixel units
    unsigned int        _viewportHeight; //Height dimension of glut window context in pixel units
    static int          _renderNow;      //Internal flag that coordinates with the framerate
    static std::mutex   _renderLock;     //Prevents write/write collisions with renderNow on a frame tick trigger

    //All keyboard input from glut will be notified here
    static void         _keyboardUpdate(unsigned char key, int x, int y);

    //One frame draw update call
    static void         _drawUpdate();

    //All mouse press input from glut will be notified here
    static void         _mouseUpdate(int button, int state, int x, int y);
    //All mouse movement input from glut will be notified here
    static void         _mouseUpdate(int x, int y);
    //Simple context synchronizes frame rate using the MasterClock tuning capability
    static void         _frameRateTrigger(int milliSeconds);
};