#include "StateVector.h"
#include "EngineManager.h"

StateVector::StateVector() : 
    _mass(1.0), 
    _active(false),
    _gravity(true) {

}

void StateVector::update(int milliSeconds) {

    //Only update kinematics if the state is in motion
    if (_active) {

        std::lock_guard<std::mutex> lockGuard(_stateLock);

        //Update kinematics here...
        float deltaTime      = static_cast<float>(milliSeconds) / 1000.0f;

        //Calculate accelerations based on external forces
        _linearAcceleration  = _force / _mass;
        _angularAcceleration = _torque / _mass;

        //Test to see if gravity is enabled
        float gravity        = GRAVITY;
        if (!_gravity) {
            gravity          = 0.0;
        }

        //Calculate linear velocity changes with delta time
        Vector4 deltaLinearVelocity(_linearAcceleration.getx() * deltaTime,
                                   (_linearAcceleration.gety() + gravity) * deltaTime,
                                    _linearAcceleration.getz() * deltaTime,
                                    1.0f);
        //Add to linear velocity
        //If there is contact with a surface then add friction coefficient to velocity
        _linearVelocity  = (_linearVelocity + deltaLinearVelocity) *
                           ((_contact || !_gravity) ? FRICTION : 1.0f);
        
        //Calculate linear position changes with delta time
        Vector4 deltaLinearPosition(_linearVelocity.getx() * deltaTime,
                                    _linearVelocity.gety() * deltaTime,
                                    _linearVelocity.getz() * deltaTime,
                                    1.0f);
        //Add to linear position
        _linearPosition  = _linearPosition + deltaLinearPosition;

        //Calculate angular velocity changes with delta time
        Vector4 deltaAngularVelocity(_angularAcceleration.getx() * deltaTime,
                                     _angularAcceleration.gety() * deltaTime,
                                     _angularAcceleration.getz() * deltaTime,
                                     1.0f);
        //Add to angular velocity
        _angularVelocity = (_angularVelocity + deltaAngularVelocity) *
                           ((_contact || !_gravity) ? FRICTION : 1.0f);

        //Calculate angular position changes with delta time
        Vector4 deltaAngularPosition(_angularVelocity.getx() * deltaTime,
                                     _angularVelocity.gety() * deltaTime,
                                     _angularVelocity.getz() * deltaTime,
                                     1.0f);
        //Add to angular position
        _angularPosition = _angularPosition + deltaAngularPosition;
    }
}

Vector4 StateVector::getLinearPosition() {
    return _linearPosition;
}

Vector4 StateVector::getAngularPosition() {
    return _angularPosition;
}

Vector4 StateVector::getLinearVelocity() {
    return _linearVelocity;
}

Vector4 StateVector::getAngularVelocity() {
    return _angularVelocity;
}

Vector4 StateVector::getLinearAcceleration() {
    return _linearAcceleration;
}

Vector4 StateVector::getAngularAcceleration() {
    return _angularAcceleration;
}

Vector4 StateVector::getForce() {
    return _force;
}

Vector4 StateVector::getTorque() {
    return _torque;
}

float StateVector::getMass() {
    return _mass;
}

bool StateVector::getActive() {
    return _active;
}

bool StateVector::getContact() {
    return _contact;
}

void StateVector::setActive(bool active) {
    std::lock_guard<std::mutex> lockGuard(_stateLock);
    _active = active;
}

void StateVector::setLinearPosition(Vector4 position) {
    std::lock_guard<std::mutex> lockGuard(_stateLock);
    _linearPosition = position;
}

void StateVector::setAngularPosition(Vector4 position) {
    std::lock_guard<std::mutex> lockGuard(_stateLock);
    _angularPosition = position;
}

void StateVector::setLinearVelocity(Vector4 velocity) {
    std::lock_guard<std::mutex> lockGuard(_stateLock);
    _linearVelocity = velocity;
}

void StateVector::setAngularVelocity(Vector4 velocity) {
    std::lock_guard<std::mutex> lockGuard(_stateLock);
    _angularVelocity = velocity;
}

void StateVector::setLinearAcceleration(Vector4 acceleration) {
    std::lock_guard<std::mutex> lockGuard(_stateLock);
    _linearAcceleration = acceleration;
}

void StateVector::setAngularAcceleration(Vector4 acceleration) {
    std::lock_guard<std::mutex> lockGuard(_stateLock);
    _angularAcceleration = acceleration;
}

void StateVector::setForce(Vector4 force) {
    std::lock_guard<std::mutex> lockGuard(_stateLock);
    //Can't set force if not touching an item to push off from i.e. in the air
    if (_contact || !_gravity) {
        _force = force;
    }
    else {
        _force = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    }
}

void StateVector::setTorque(Vector4 torque) {
    std::lock_guard<std::mutex> lockGuard(_stateLock);
    //Can't set torque if not touching an item to push off from i.e. in the air
    if (_contact || !_gravity) {
        _torque = torque;
    }
    else {
        _torque = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    }
}

void StateVector::setContact(bool contact) {
    std::lock_guard<std::mutex> lockGuard(_stateLock);
    _contact = contact;
}

void StateVector::setGravity(bool enableGravity) {
    std::lock_guard<std::mutex> lockGuard(_stateLock);
    _gravity = enableGravity;
}
