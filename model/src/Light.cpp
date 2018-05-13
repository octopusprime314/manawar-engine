#include "Light.h"
#include "MasterClock.h"

Light::Light(ViewManagerEvents* eventWrapper, 
    MVP mvp, LightType type, Vector4 color, bool shadowCaster) :
    UpdateInterface(eventWrapper),
    _type(type),
    _lightMVP(mvp),
    _color(color),
    _shadowCaster(shadowCaster),
    _milliSecondTime(0){

    MasterClock::instance()->subscribeKinematicsRate(std::bind(&Light::_updateTime, this, std::placeholders::_1));
    
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

bool Light::isShadowCaster() {
    return _shadowCaster;
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

void Light::_updateTime(int time) {

    //The amount of milliseconds in 24 hours
    const uint64_t dayLengthMilliseconds = 24 * 60 * 60 * 1000;

    //Every update comes in real time so in order to speed up
    //we need to multiply that value by some constant
    //A full day takes one minute should do it lol
    //divide total time by 60 seconds times 1000 to convert to milliseconds
    uint64_t updateTimeAmplified = dayLengthMilliseconds / (60 * 1000);

    _milliSecondTime += (updateTimeAmplified*time);
    _milliSecondTime %= dayLengthMilliseconds;
    
    if (_type == LightType::MAP_DIRECTIONAL || _type == LightType::CAMERA_DIRECTIONAL) {

        //fraction of the rotation
        float posInRotation = static_cast<float>(_milliSecondTime) / static_cast<float>(dayLengthMilliseconds);
        
        float* inverseModel = _lightMVP.getViewMatrix().getFlatBuffer();
        float radiusOfLight = Vector4(inverseModel[3], inverseModel[7], inverseModel[11], 1.0f).getMagnitude();
        _lightMVP.setView(Matrix::cameraTranslation(0.0, 0.0, radiusOfLight) * 
            Matrix::cameraRotationAroundX(-90.0f + (posInRotation*360.0f)));
        
        //std::cout << x << " " << y << std::endl;
        //std::cout << _milliSecondTime << std::endl;
    }
}


