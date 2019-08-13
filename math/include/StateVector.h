/*
* StateVector is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  StateVector class. Keeps track of kinematic state.
*/

#pragma once
#include "Matrix.h"
#include "Vector4.h"
#include <mutex>

//meters per second squared
const float GRAVITY = -9.8f;
//friction coefficient applied
const float FRICTION = 0.95f;

class StateVector {
    Vector4    _angularAcceleration;
    Vector4    _linearAcceleration;
    Vector4    _angularVelocity;
    Vector4    _angularPosition;
    Vector4    _linearPosition;
    Vector4    _linearVelocity;
    std::mutex _stateLock;
    bool       _contact;
    bool       _gravity;
    Vector4    _torque;
    bool       _active;
    Vector4    _force;
    float      _mass;

public:
    StateVector();
    void       setAngularAcceleration(Vector4 acceleration);
    void       setLinearAcceleration( Vector4 acceleration);
    void       setAngularPosition(    Vector4 position);
    void       setAngularVelocity(    Vector4 velocity);
    void       setLinearVelocity(     Vector4 velocity);
    void       setLinearPosition(     Vector4 position);
    void       setContact(            bool    contact);
    void       setGravity(            bool    enableGravity);
    void       setActive(             bool    active);
    void       setTorque(             Vector4 torque);
    void       setForce(              Vector4 force);
    void       update(                int     milliSeconds);

    Vector4    getAngularAcceleration();
    Vector4    getLinearAcceleration();
    Vector4    getAngularPosition();
    Vector4    getAngularVelocity();
    Vector4    getLinearPosition();
    Vector4    getLinearVelocity();
    bool       getContact();
    bool       getActive();
    Vector4    getTorque();
    Vector4    getForce();
    float      getMass();
};