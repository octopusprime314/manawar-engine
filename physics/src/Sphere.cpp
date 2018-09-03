#include "Sphere.h"
#include "GeometryMath.h"

Sphere::Sphere(float radius, Vector4 position) :
    _radius(radius),
    _position(position) {
}

Sphere::~Sphere() {
}

float Sphere::getRadius() {
    Sphere transformed = GeometryMath::transform(this, _modelTransform);
    return transformed._radius;
    //return _radius;
}

Vector4 Sphere::getPosition() {

    Sphere transformed = GeometryMath::transform(this, _modelTransform);
    return transformed._position;
    //return _position;
}

Vector4 Sphere::getObjectPosition() {
    return _position;
}

float Sphere::getObjectRadius() {
    return _radius;
}

void Sphere::offsetTransform(Matrix transform) {
    _modelTransform = transform;
}
