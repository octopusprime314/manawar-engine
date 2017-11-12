#include "MasterClock.h"

MasterClock* MasterClock::_clock = nullptr;

MasterClock::MasterClock() : _milliSecondCounter(0){

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

    //Run the master clock process that is responsible for sending time events to subscribers
    _clockThread = new std::thread(&MasterClock::_clockProcess, _clock);

}
void MasterClock::_clockProcess(){

    while(true){

        //If the millisecond amount is divisible by frame time then trigger a frame time event to subscribers
        if(_milliSecondCounter % FRAME_TIME == 0){
            for(auto funcs : _frameRateFuncs){
                funcs(FRAME_TIME);
            }
        }

        //If the millisecond amount is divisible by kinematics time then trigger a kinematic calculation time event to subscribers
        if(_milliSecondCounter % KINEMATICS_TIME == 0){
            for(auto funcs : _kinematicsRateFuncs){
                funcs(KINEMATICS_TIME);
            }
        }

        //Wait for a millisecond
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); //1 millisecond clock resolution
        _milliSecondCounter++;
    }
}

void MasterClock::subscribeFrameRate(std::function<void(int)> func){
    _frameRateFuncs.push_back(func);
}

void MasterClock::subscribeKinematicsRate(std::function<void(int)> func){
    _kinematicsRateFuncs.push_back(func);
}
