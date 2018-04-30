#include "Light.h"

Light::Light(ViewManagerEvents* eventWrapper, 
    MVP mvp, LightType type, Vector4 color, float range) :
    UpdateInterface(eventWrapper),
    _type(type),
    _mvp(mvp),
    _color(color),
    _range(range) {
    
    //Extract light position from view matrix
    float* inverseView = _mvp.getViewMatrix().inverse().getFlatBuffer();
    _position = Vector4(inverseView[3], inverseView[7], inverseView[11], 1.0);
}

MVP Light::getMVP() {

    //Move the positions of the lights based on the camera view except
    //the large map directional light that is used for low resolution 
    //shadow map generation
    if (_type == LightType::MAP_DIRECTIONAL) {
        return _mvp;
    }
    else {
        MVP temp = _mvp;
        float* inverseView = _view.inverse().getFlatBuffer();
        temp.setView(_mvp.getViewMatrix() *
            Matrix::cameraTranslation(inverseView[3], inverseView[7], inverseView[11]));
        return temp;
    }
}

Vector4 Light::getPosition() {
    return _view * _position;
}

LightType Light::getType(){
    return _type;
}

Vector4& Light::getColor() {
    return _color;
}

float Light::getRange() {
    return _range;
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
    _view = view;
}

void Light::_updateProjection(Matrix projection) {
    _projection = projection;
}


