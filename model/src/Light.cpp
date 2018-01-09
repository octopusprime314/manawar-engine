#include "Light.h"
ViewManagerEvents* Light::_viewEventWrapper;

Light::Light(Vector4 lightDir) {

    _lightDir = lightDir;

    //Subscribe to view updates to transform lights properly
    _viewEventWrapper->subscribeToView(std::bind(&Light::_updateView, this, std::placeholders::_1));
}


void Light::_updateView(Matrix view) {

    _mvp.setView(view); //Receive updates when the view matrix has changed

    //If view changes then change our normal matrix
    _mvp.setNormal(view.inverse().transpose());
}

Vector4 Light::getLightDirection() {
    return _lightDir;
}

float* Light::getNormalBuffer() {
    return _mvp.getNormalBuffer();
}

Matrix Light::getNormalMatrix(){
    return _mvp.getNormalMatrix();
}

void Light::setViewWrapper(ViewManagerEvents* wrapper){
    _viewEventWrapper = wrapper;
}
