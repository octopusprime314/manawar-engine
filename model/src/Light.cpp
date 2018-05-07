#include "Light.h"

Light::Light(ViewManagerEvents* eventWrapper, 
    MVP mvp, LightType type, Vector4 color) :
    UpdateInterface(eventWrapper),
    _type(type),
    _lightMVP(mvp),
    _color(color) {
    
    //Extract light position from view matrix
    float* inverseView = _lightMVP.getViewMatrix().inverse().getFlatBuffer();
    _position = Vector4(inverseView[3], inverseView[7], inverseView[11], 1.0);
}

MVP Light::getMVP() {

    //Move the positions of the lights based on the camera view except
    //the large map directional light that is used for low resolution 
    //shadow map generation
    if (_type == LightType::MAP_DIRECTIONAL) {
        return _lightMVP;
    }
    else {
        MVP temp = _lightMVP;
       float* inverseView = _cameraMVP.getViewMatrix().inverse().getFlatBuffer();
        temp.setView(_lightMVP.getViewMatrix() *
            Matrix::cameraTranslation(inverseView[3], inverseView[7], inverseView[11]));
        return temp;
    }
}

Vector4 Light::getPosition() {
    //Extract light position from model matrix
    float* inverseModel = _lightMVP.getModelMatrix().getFlatBuffer();
    _position = Vector4(inverseModel[3], inverseModel[7], inverseModel[11], 1.0);
    return _position;
}

LightType Light::getType(){
    return _type;
}

Vector4& Light::getColor() {
    return _color;
}

float Light::getRange() {

    auto projMatrix = _lightMVP.getProjectionBuffer();
    float nearVal = (2.0f*projMatrix[11]) / (2.0f*projMatrix[10] - 2.0f);
    float farVal = ((projMatrix[10] - 1.0f)*nearVal) / (projMatrix[10] + 1.0f);
    return farVal;
}

void Light::setMVP(MVP mvp){
    _lightMVP = mvp;
}

void Light::_updateDraw() {
}
void Light::_updateKeyboard(int key, int x, int y) {
}
void Light::_updateReleaseKeyboard(int key, int x, int y) {
}
void Light::_updateMouse(double x, double y) {
}

void Light::_updateView(Matrix view) {
    _cameraMVP.setView(view);
}

void Light::_updateProjection(Matrix projection) {
    _cameraMVP.setProjection(projection);
}


