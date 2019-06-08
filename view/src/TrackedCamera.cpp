#define _USE_MATH_DEFINES
#include "TrackedCamera.h"
#include "Logger.h"
#include "EngineManager.h"

TrackedCamera::TrackedCamera()
    : Camera()
    , _currentWaypoint(-1)
{

}

void TrackedCamera::reset() {
    auto state = getState();
    // Set to some initialization
    _currentWaypoint = -1;

    if (_waypoints.size() >= 2) {
        _currentWaypoint = 0;
        state->setLinearPosition(_waypoints[0].position);
        auto current_wp = _waypoints[0].position;
        auto next_wp = _waypoints[1].position;
        _currentForce = (next_wp - current_wp);
        _currentForce.normalize();

        state->setForce(_inversion * _currentForce * 100.0f);

    }
}

void TrackedCamera::setInversion(const Matrix& inversion) {
    _inversion = inversion;
}

void TrackedCamera::setWaypoints(const std::vector<CameraWaypoint>& waypoints) {
    _waypoints = waypoints;
    reset();
}

void TrackedCamera::updateState(int milliseconds) {
    auto state = getState();
    
    if (_currentWaypoint != -1) {
        int next_wp = _currentWaypoint + 1;
        auto current_pos = state->getLinearPosition();

        auto dir = _waypoints[next_wp].position - current_pos;
        float mag = dir.getMagnitude();
        
        if (mag <= 0.5f ) {
            _currentWaypoint += 1;
            if ( _currentWaypoint >= _waypoints.size() - 1 ) {
                _currentWaypoint = -1;
                _currentForce = Vector4(0, 0, 0);
            }
            else {
                auto current_wp = _waypoints[_currentWaypoint].position;
                auto next_wp = _waypoints[_currentWaypoint+1].position;
                
                _currentForce = (next_wp - current_wp);
                
                _currentForce.normalize();     

                state->setLinearPosition(_waypoints[_currentWaypoint].position);
            }
            state->setForce(_inversion * _currentForce * 100.0f);

        }
    }
    
    state->update(milliseconds);

}