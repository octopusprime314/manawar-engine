#include "MasterClock.h"
#include <ctime>
#include <iostream>
MasterClock* MasterClock::_clock = nullptr;

MasterClock::MasterClock() : 
    _frameTime(    DEFAULT_FRAME_TIME),
    _animationTime(DEFAULT_FRAME_TIME) {

}

MasterClock::~MasterClock() {

}

MasterClock* MasterClock::instance() {
    if (_clock == nullptr) {
        _clock = new MasterClock();
    }
    return _clock;
}

void MasterClock::run() {

    //Run the clock event processes that are responsible for sending time events to subscribers
    _physicsThread   = new std::thread(&MasterClock::_physicsProcess,   _clock);
    _fpsThread       = new std::thread(&MasterClock::_fpsProcess,       _clock);
    _animationThread = new std::thread(&MasterClock::_animationProcess, _clock);
}

void MasterClock::_physicsProcess() {
    int milliSecondCounter = 0;
    while (true) {
        auto start = std::chrono::high_resolution_clock::now();
        //If the millisecond amount is divisible by kinematics time,
        //then trigger a kinematic calculation time event to subscribers
        if (milliSecondCounter == KINEMATICS_TIME) {
            _kinematicsRateLock.lock();
            milliSecondCounter = 0;
            for (auto func : _kinematicsRateFuncs) {
                if (func != nullptr) {
                    func(KINEMATICS_TIME);
                }
            }
            _kinematicsRateLock.unlock();
        }
        auto end            = std::chrono::high_resolution_clock::now();
        double milliseconds = std::chrono::duration<double, std::milli>(end - start).count();
        int deltaTime       = static_cast<int>(static_cast<double>(KINEMATICS_TIME) - milliseconds);
        if (deltaTime > 0) {
            //std::cout << "left over time: " << deltaTime << std::endl;
            //Wait for remaining milliseconds
            std::this_thread::sleep_for(std::chrono::milliseconds(deltaTime));
        }
        else if (deltaTime < 0) {
            std::cout << "Extra time being used on physics calculations: "
                      << milliseconds - KINEMATICS_TIME << std::endl;
        }
        milliSecondCounter += KINEMATICS_TIME;
    }
}

void MasterClock::_fpsProcess() {
    int milliSecondCounter = 0;
    while (true) {
        auto start = std::chrono::high_resolution_clock::now();
        //If the millisecond amount is divisible by frame time then trigger a frame time event to subscribers
        if (milliSecondCounter == _frameTime) {
            _frameRateLock.lock();
            milliSecondCounter = 0;
            for (auto func : _frameRateFuncs) {
                if (func != nullptr) {
                    func(_frameTime);
                }
            }
            _frameRateLock.unlock();
        }
        auto end = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration<double, std::milli>(end - start).count() <= 1.0f) {
            //Wait for a millisecond
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            milliSecondCounter++;
        }
    }
}

void MasterClock::_animationProcess() {
    int milliSecondCounter = 0;
    while (true) {
        auto start = std::chrono::high_resolution_clock::now();
        //If the millisecond amount is divisible by frame time then trigger a frame time event to subscribers
        if (milliSecondCounter == _animationTime) {
            _animationRateLock.lock();
            milliSecondCounter = 0;
            for (auto func : _animationRateFuncs) {
                if (func != nullptr) {
                    func(_animationTime);
                }
            }
            _animationRateLock.unlock();
        }
        auto end = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration<double, std::milli>(end - start).count() <= 1.0f) {
            //Wait for a millisecond
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            milliSecondCounter++;
        }
    }
}

void MasterClock::setFrameRate(int framesPerSecond) {
    _frameTime = static_cast<int>((1.0 / static_cast<double>(framesPerSecond)) * 1000.0);
}

void MasterClock::subscribeFrameRate(std::function<void(int)> func) {
    _frameRateLock.lock();
    _frameRateFuncs.push_back(func);
    _frameRateLock.unlock();
}

void MasterClock::subscribeAnimationRate(std::function<void(int)> func) {
    _animationRateLock.lock();
    _animationRateFuncs.push_back(func);
    _animationRateLock.unlock();
}

void MasterClock::subscribeKinematicsRate(std::function<void(int)> func) {
    _kinematicsRateLock.lock();
    _kinematicsRateFuncs.push_back(func);
    _kinematicsRateLock.unlock();
}
