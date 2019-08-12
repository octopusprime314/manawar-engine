#include "EventSubscriber.h"
#include "IOEvents.h"
#include "ViewEventDistributor.h"

EventSubscriber::EventSubscriber() {
    //Input events
    IOEvents::subscribeToKeyboard(       std::bind(&EventSubscriber::_updateKeyboard, this, _1, _2, _3));
    IOEvents::subscribeToReleaseKeyboard(std::bind(&EventSubscriber::_updateReleaseKeyboard, this, _1, _2, _3));
    IOEvents::subscribeToMouse(          std::bind(&EventSubscriber::_updateMouse, this, _1, _2));
    IOEvents::subscribeToDraw(           std::bind(&EventSubscriber::_updateDraw, this));
    IOEvents::subscribeToGameState(      std::bind(&EventSubscriber::_updateGameState, this, _1));
}

EventSubscriber::EventSubscriber(ViewEvents* eventWrapper) {
    //Input events
    IOEvents::subscribeToKeyboard(       std::bind(&EventSubscriber::_updateKeyboard, this, _1, _2, _3));
    IOEvents::subscribeToReleaseKeyboard(std::bind(&EventSubscriber::_updateReleaseKeyboard, this, _1, _2, _3));
    IOEvents::subscribeToMouse(          std::bind(&EventSubscriber::_updateMouse, this, _1, _2));
    IOEvents::subscribeToDraw(           std::bind(&EventSubscriber::_updateDraw, this));
    IOEvents::subscribeToGameState(      std::bind(&EventSubscriber::_updateGameState, this, _1));

    //View/Camera events
    eventWrapper->subscribeToView(      std::bind(&EventSubscriber::_updateView, this, _1));
    eventWrapper->subscribeToProjection(std::bind(&EventSubscriber::_updateProjection, this, _1));
}

EventSubscriber::~EventSubscriber() {}
void EventSubscriber::_updateView(Matrix view) {}
void EventSubscriber::_updateProjection(Matrix view) {}
