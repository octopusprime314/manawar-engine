#define _USE_MATH_DEFINES
#include "VectorCamera.h"
#include "Logger.h"
#include "EngineManager.h"
#include "IOEventDistributor.h"
#include <fstream>
#include <iterator>
#include <sstream>

VectorCamera::VectorCamera()
    : Camera()
{

}

void VectorCamera::reset() {
    auto state = getState();
    _currentVector = -1;
    if (_vectors.size() > 0) {
        _elapsedTime = 0;
        _currentVector = 0;
        state->setForce(_inversion *_vectors[_currentVector].direction);
        state->setTorque(_vectors[_currentVector].rotation);
    }
}

void VectorCamera::setInversion(const Matrix& inversion) {
    _inversion = inversion;
}

void VectorCamera::setVectors(const std::vector<CameraVector>& waypoints) {
    _vectors = waypoints;
    reset();
}

void VectorCamera::setVectorsFromFile(const std::string& file)
{
    std::ifstream infile(file);
    _vectors.clear();
    reset();

    double v_x, v_y, v_z, r_x, r_y, r_z;
    float time;
    std::string line;
    while(std::getline(infile, line))
    {

        std::stringstream ss(line);
        std::istream_iterator<std::string> begin(ss);
        std::istream_iterator<std::string> end;
        std::vector<std::string> vstrings(begin, end);
        std::copy(vstrings.begin(), vstrings.end(), std::ostream_iterator<std::string>(std::cout, "\n"));
        
        v_x = ::atof(vstrings[0].c_str());
        v_y = ::atof(vstrings[1].c_str());
        v_z = ::atof(vstrings[2].c_str());

        r_x = ::atof(vstrings[3].c_str());
        r_y = ::atof(vstrings[4].c_str());
        r_z = ::atof(vstrings[5].c_str());

        time = static_cast<float>(::atof(vstrings[6].c_str()));
        Vector4 d(static_cast<float>(v_x), static_cast<float>(v_y), static_cast<float>(v_z));
        Vector4 r(static_cast<float>(r_x), static_cast<float>(r_y), static_cast<float>(r_z));
        _vectors.emplace_back(d, r, time);
        
    }
    reset();
}

void VectorCamera::updateState(int milliseconds) {
    auto state = getState();

    if (_currentVector != -1 ) {     
        if (_elapsedTime >= _vectors[_currentVector].time) {
            _elapsedTime = 0;
            _currentVector++;

            if (_currentVector >= _vectors.size()) {
                _currentVector = -1;
                state->setForce(Vector4(0.0, 0.0, 0.0));
                state->setLinearVelocity(Vector4(0.0, 0.0, 0.0));
                state->setTorque(Vector4(0.0, 0.0, 0.0));
                IOEventDistributor::quit();
            }
            else {
                state->setForce(_inversion * _vectors[_currentVector].direction);             
            }
        }
        _elapsedTime += milliseconds;
    }
    state->setTorque(_vectors[_currentVector].rotation);
    state->update(milliseconds);
}