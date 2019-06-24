#define _USE_MATH_DEFINES
#include "WaypointCamera.h"
#include "Logger.h"
#include "EngineManager.h"

WaypointCamera::WaypointCamera()
    : Camera()
    , _currentWaypoint(-1)
{

}

void WaypointCamera::reset() {
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

void WaypointCamera::setInversion(const Matrix& inversion) {
    _inversion = inversion;
}

void WaypointCamera::setWaypoints(const std::vector<CameraWaypoint>& waypoints) {
    _waypoints = waypoints;
    reset();
}

void WaypointCamera::updateState(int milliseconds) {
    auto state = getState();
    
    if (_currentWaypoint != -1) {
        auto current_pos = state->getLinearPosition();
        auto next_wp = _waypoints[_currentWaypoint + 1].position;
        auto prev_wp = _waypoints[_currentWaypoint].position;
       
        LOG_TRACE(current_pos.getz());
        auto dir = next_wp - current_pos;
        float mag = dir.getMagnitude();

        LOG_TRACE(mag);
        if (mag <= 1.0f ) {
            _currentWaypoint += 1;
            if ( _currentWaypoint >= _waypoints.size() - 1 ) {
                _currentWaypoint = -1;
                _currentForce = Vector4(0, 0, 0);
            }
            else {
                auto current_wp = _waypoints[_currentWaypoint].position;
                auto& next_wp = _waypoints[_currentWaypoint+1].position;
                
                _currentForce = (next_wp - current_wp);
                
                _currentForce.normalize();   
                LOG_TRACE(_currentForce.getx());
                state->setLinearPosition(current_wp);
            }
            state->setForce(_inversion * _currentForce * 100.0f);

        }
    }
    
    state->update(milliseconds);

}