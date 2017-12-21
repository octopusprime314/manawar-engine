#include "MasterClock.h"
#include <ctime>
#include <iostream>
MasterClock* MasterClock::_clock = nullptr;

MasterClock::MasterClock() : _frameTime(DEFAULT_FRAME_TIME),
    _animationTime(DEFAULT_FRAME_TIME){

}

MasterClock::~MasterClock(){

}

MasterClock* MasterClock::instance(){
    if(_clock == nullptr){
        _clock = new MasterClock();
    }
    return _clock;
}

void MasterClock::run(){

    //Run the clock event processes that are responsible for sending time events to subscribers
    _physicsThread = new std::thread(&MasterClock::_physicsProcess, _clock);
    _fpsThread = new std::thread(&MasterClock::_fpsProcess, _clock);
    _animationThread = new std::thread(&MasterClock::_animationProcess, _clock);
}

void MasterClock::_physicsProcess(){
    int milliSecondCounter = 0;
    while(true){
        auto start = std::chrono::high_resolution_clock::now();
        //If the millisecond amount is divisible by kinematics time then trigger a kinematic calculation time event to subscribers
        if(milliSecondCounter == KINEMATICS_TIME){
            milliSecondCounter = 0;
            for(auto funcs : _kinematicsRateFuncs){
                funcs(KINEMATICS_TIME);
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration<double, std::milli>(end - start).count() <= 1.0f) {
            //Wait for a millisecond
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); //1 millisecond clock resolution
            milliSecondCounter++;
        }
        else {
            //std::cout << "Extra time being used on physics calculations: " << std::chrono::duration<double, std::milli>(end - start).count() << std::endl;
        }
    }
}

void MasterClock::_fpsProcess(){
    int milliSecondCounter = 0;
    while(true){
        auto start = std::chrono::high_resolution_clock::now();
        //If the millisecond amount is divisible by frame time then trigger a frame time event to subscribers
        if(milliSecondCounter == _frameTime){
            milliSecondCounter = 0;
            for(auto funcs : _frameRateFuncs){
                funcs(_frameTime);
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration<double, std::milli>(end - start).count() <= 1.0f) {
            //Wait for a millisecond
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); //1 millisecond clock resolution
            milliSecondCounter++;
        }
    }
}

void MasterClock::_animationProcess(){
    int milliSecondCounter = 0;
    while(true){
        auto start = std::chrono::high_resolution_clock::now();
        //If the millisecond amount is divisible by frame time then trigger a frame time event to subscribers
        if(milliSecondCounter == _animationTime){
            milliSecondCounter = 0;
            for(auto funcs : _animationRateFuncs){
                funcs(_animationTime);
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration<double, std::milli>(end - start).count() <= 1.0f) {
            //Wait for a millisecond
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); //1 millisecond clock resolution
            milliSecondCounter++;
        }
    }
}

void MasterClock::setFrameRate(int framesPerSecond) {
    _frameTime = static_cast<int>((1.0/static_cast<double>(framesPerSecond)) * 1000.0);
}

void MasterClock::subscribeFrameRate(std::function<void(int)> func){
    _frameRateFuncs.push_back(func);
}

void MasterClock::subscribeAnimationRate(std::function<void(int)> func){
    _animationRateFuncs.push_back(func);
}

void MasterClock::subscribeKinematicsRate(std::function<void(int)> func){
    _kinematicsRateFuncs.push_back(func);
}
