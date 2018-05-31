#include "ViewManagerEvents.h"

namespace Factory {
    extern ViewManagerEvents* _viewEventWrapper = nullptr;
}

void ViewManagerEvents::subscribeToView(std::function<void(Matrix)> func) { //Use this call to connect functions to camera/view updates
    _viewFuncs.push_back(func);
}
void ViewManagerEvents::subscribeToProjection(std::function<void(Matrix)> func) { //Use this call to connect functions to projection updates
    _projectionFuncs.push_back(func);
}

//Blast all subscribers that have overriden the updateView function
void ViewManagerEvents::updateView(Matrix view) {
    for (auto func : _viewFuncs) {
        func(view); //Call view/camera update 
    }
}
//Blast all subscribers that have overriden the updateProjection function
void ViewManagerEvents::updateProjection(Matrix proj) {
    for (auto func : _projectionFuncs) {
        func(proj); //Call projection update 
    }
}