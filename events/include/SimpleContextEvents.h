/*
* SimpleContextEvents is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  SimpleContextEvents class. Input event functions to be overriden
*/

#pragma once
#include <vector>
#include <functional>
#include "Matrix.h"
#include "EngineState.h"

struct GLFWwindow;

class SimpleContextEvents {

    static std::vector<std::function<void(int, int, int)>>           _keyboardFuncs;
    static std::vector<std::function<void(int, int, int)>>           _keyboardReleaseFuncs;
    static std::vector<std::function<void(int, int, int, int)>>      _mouseButtonFuncs;
    static std::vector<std::function<void(double, double)>>          _mouseFuncs;
    static std::vector<std::function<void()>>                        _drawFuncs;
    static std::vector<std::function<void(EngineStateFlags)>>             _gameStateFuncs;
    static std::function<void()>                                     _preDrawCallback; //Prior to drawing objects call this function
    static std::function<void()>                                     _postDrawCallback; //Post of drawing objects call this function

public:
    static void subscribeToKeyboard(std::function<void(int, int, int)> func); //Use this call to connect functions up to key updates
    static void subscribeToReleaseKeyboard(std::function<void(int, int, int)> func); //Use this call to connect functions up to key updates
    static void subscribeToMouseClick(std::function<void(int, int, int, int)> func); //Use this call to connect functions to mouse button updates
    static void subscribeToMouse(std::function<void(double, double)> func); //Use this call to connect functions up to mouse updates
    static void subscribeToDraw(std::function<void()> func); //Use this call to connect functions up to draw updates
    static void subscribeToGameState(std::function<void(EngineStateFlags)> func);

    static void updateKeyboard(int key, int x, int y);
    static void releaseKeyboard(int key, int x, int y);
    static void updateDraw(GLFWwindow* _window);
    static void updateMouse(double x, double y);
    static void updateMouseClick(int button, int action, int x, int y);
    static void updateGameState(EngineStateFlags state);

    static void setPreDrawCallback(std::function<void()> func);
    static void setPostDrawCallback(std::function<void()> func);
};