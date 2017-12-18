#include "StateVector.h"

StateVector::StateVector() : _mass(1.0), _active(false) {

}

void StateVector::update(int milliSeconds) {

    //Only update kinematics if the state is in motion
    if (_active) {
        //Update kinematics here...
        float deltaTime = static_cast<float>(milliSeconds) / 1000.0f; //Convert to fraction of a second

        //Calculate accelerations based on external forces

        _linearAcceleration = _force / _mass;
        _angularAcceleration = _torque / _mass;

        //Calculate linear velocity changes with delta time
        Vector4 deltaLinearVelocity(_linearAcceleration.getx() * deltaTime,
            (_linearAcceleration.gety() + GRAVITY) * deltaTime,
            _linearAcceleration.getz() * deltaTime,
            1.0f);
        //Add to linear velocity
        _linearVelocity = _linearVelocity + deltaLinearVelocity;

        //Calculate linear position changes with delta time
        Vector4 deltaLinearPosition(_linearVelocity.getx() * deltaTime,
            _linearVelocity.gety() * deltaTime,
            _linearVelocity.getz() * deltaTime,
            1.0f);
        //Add to linear position
        _linearPosition = _linearPosition + deltaLinearPosition;

        //Calculate angular velocity changes with delta time
        Vector4 deltaAngularVelocity(_angularAcceleration.getx() * deltaTime,
            _angularAcceleration.gety() * deltaTime,
            _angularAcceleration.getz() * deltaTime,
            1.0f);
        //Add to angular velocity
        _angularVelocity = _angularVelocity + deltaAngularVelocity;

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

void StateVector::setActive(bool active) {
    _active = active;
}

void StateVector::setLinearPosition(Vector4 position) {
    _linearPosition = position;
}

void StateVector::setAngularPosition(Vector4 position){
    _angularPosition = position;
}

void StateVector::setLinearVelocity(Vector4 velocity){
    _linearVelocity = velocity;
}

void StateVector::setAngularVelocity(Vector4 velocity){
    _angularVelocity = velocity;
}

void StateVector::setLinearAcceleration(Vector4 acceleration){
    _linearAcceleration = acceleration;
}

void StateVector::setAngularAcceleration(Vector4 acceleration){
    _angularAcceleration = acceleration;
}
