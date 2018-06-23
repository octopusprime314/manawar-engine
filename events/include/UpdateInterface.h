/*
* UpdateInterface is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  UpdateInterface class.  Class to derive from if a class wants to receive
*  input and view manager events.  Dynamic models will care about mouse and keyboard
*  events while static models that don't move will essentially no-op.
*/

#pragma once
#include <functional>
#include "Matrix.h"
using namespace std::placeholders;

class ViewManagerEvents;

//Class needed to derive from to subscribe to input events
class UpdateInterface {

public:
    UpdateInterface();
    UpdateInterface(ViewManagerEvents* eventWrapper);
    virtual ~UpdateInterface();

protected:
    virtual void _updateKeyboard(int key, int x, int y) = 0;
    virtual void _updateReleaseKeyboard(int key, int x, int y) = 0;
    virtual void _updateMouse(double x, double y) = 0;
    virtual void _updateDraw() = 0;
    virtual void _updateView(Matrix view); //optional to implement overriden function
    virtual void _updateProjection(Matrix view); //optional to implement overriden function
};