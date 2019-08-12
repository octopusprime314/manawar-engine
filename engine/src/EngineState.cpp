#include "EngineState.h"

EngineStateFlags EngineState::_engineStateFlags = {false,
                                                   false,
                                                   false,
                                                   false,
                                                   false,
                                                   true};

EngineState::EngineState() {
}

EngineState::~EngineState() {
}

const EngineStateFlags& EngineState::getEngineState(){
    return _engineStateFlags;
}

void EngineState::setEngineState(EngineStateFlags state) {
    _engineStateFlags = state;
}
