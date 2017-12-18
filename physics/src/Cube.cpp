#include "Cube.h"

Cube::Cube(float length, float width, float height, Vector4 center) :
    _length(length),
    _width(width),
    _height(height),
    _center(center) {

}

float Cube::getLength() {
    return _length;
}
float Cube::getHeight() {
    return _length;
}
float Cube::getWidth() {
    return _length;
}
Vector4 Cube::getCenter() {
    return _center;
}