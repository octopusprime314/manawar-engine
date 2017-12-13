#include "StateVector.h"

StateVector::StateVector() : _mass(1.0) {

}

void StateVector::update(int milliSeconds){
    //Update kinematics here...
    float deltaTime = static_cast<float>(milliSeconds) / 1000.0f; //Convert to fraction of a second

    //Calculate accelerations based on external forces
    _linearAcceleration =  _force  / _mass;
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