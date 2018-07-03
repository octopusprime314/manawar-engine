/*
* FunctionState is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  FunctionState class. Takes a lamda as an mathematical expression
*  and keeps track of the state of the function.  For example if a
*  function f(t) is created for an acceleration model then internally
*  time must be kept track of.
*/

//Example usage: lasts for 3000 milliseconds in intervals of 5 milliseconds passing a function lambda of e^-4t and func pointer
//          FunctionState(std::bind(&StateVector::setForce, state, std::placeholders::_1), 
//                        [=](double t) -> Vector4 { return static_cast<Vector4>(force) * (powf(M_E, -4.0*t));}, 
//                        5, 
//                        3000); 
#include <functional>
#include <thread>
#include <iostream>
#include <chrono>
#include "Vector4.h"

class FunctionState {

    std::thread _lambdaThread; //Lambda thread
    bool        _terminate;
    Vector4     _currValue;
public:

    FunctionState(std::function<void(Vector4)> functionPointer, std::function<Vector4(float)> equationLambda,
        int milliseconds, int duration = INT_MAX) : _terminate(false) {

        //time, function pointer and milliseconds timer update
        _lambdaThread = std::thread([=]() {

            Vector4 ft(0.0, 0.0, 0.0, 1.0);
            int t = 0;
            float scalar = 1.0f;
            do {

                //compute milliseconds to a fraction
                float seconds = t / 1000.0f;

                //Calculate new equation value
                _currValue = equationLambda(seconds);

                //Call function pointer
                functionPointer(_currValue);

                //sleep for the designated timer interval
                std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));

                //increment equation time
                t += milliseconds;

            } while (t < duration && !_terminate); //Stop updates and kill lambda thread when equation has reached a threshold value

            functionPointer(Vector4(0.0f, 0.0f, 0.0f, 1.0f)); //kill acceleration
        });
    }
    ~FunctionState() {
        _terminate = true;
        _lambdaThread.join();
    }

    Vector4 getVectorState() {
        return _currValue;
    }
};