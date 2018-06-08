#include "Sphere.h"

Sphere::Sphere(float radius, Vector4 position) :
    _radius(radius),
    _position(position) {

}

Sphere::~Sphere() {

}

float Sphere::getRadius(){
    return _radius;
}

Vector4 Sphere::getPosition(){
    return _position + _modelPosition;
}

void Sphere::offsetPosition(Vector4 position){
    _modelPosition = position;
}
