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

class SimpleContextEvents {

    static std::vector<std::function<void(unsigned char, int, int)>> _keyboardFuncs;
	static std::vector<std::function<void(unsigned char, int, int)>> _keyboardReleaseFuncs;
    static std::vector<std::function<void(int, int, int, int)>>      _mouseFuncs;
    static std::vector<std::function<void()>>                        _drawFuncs;
    static std::function<void()>                                     _preDrawCallback; //Prior to drawing objects call this function
    static std::function<void()>                                     _postDrawCallback; //Post of drawing objects call this function
    
public:
    static void subscribeToKeyboard(std::function<void(unsigned char, int, int)> func); //Use this call to connect functions up to key updates
	static void subscribeToReleaseKeyboard(std::function<void(unsigned char, int, int)> func); //Use this call to connect functions up to key updates
    static void subscribeToMouse(std::function<void(int, int, int, int)> func); //Use this call to connect functions up to mouse updates
    static void subscribeToDraw(std::function<void()> func); //Use this call to connect functions up to draw updates

    static void updateKeyboard(unsigned char key, int x, int y);
	static void releaseKeyboard(unsigned char key, int x, int y);
    static void updateDraw();
    static void updateMouse(int button, int state, int x, int y);
    static void updateMouse(int x, int y);

    static void setPreDrawCallback(std::function<void()> func);
    static void setPostDrawCallback(std::function<void()> func);
};