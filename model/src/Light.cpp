#include "Light.h"

Light::Light(MVP mvp, LightType type) {
    
    _type = type;
    _mvp = mvp;

    //Extract light position from view matrix
    float* inverseView = _mvp.getViewMatrix().inverse().getFlatBuffer();
    _position = Vector4(inverseView[3], inverseView[7], inverseView[11], 1.0);
}

MVP& Light::getMVP() {
    return _mvp;
}

Vector4& Light::getPosition() {
    return _position;
}

LightType Light::getType(){
    return _type;
}


