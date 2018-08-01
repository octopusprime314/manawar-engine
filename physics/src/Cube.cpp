#include "Cube.h"

Cube::Cube(float length, float height, float width, Vector4 center) :
    _length(length),
    _width(width),
    _height(height),
    _center(center) {

}

float Cube::getLength() {
    return _length;
}
float Cube::getHeight() {
    return _height;
}
float Cube::getWidth() {
    return _width;
}
Vector4 Cube::getCenter() {
    return _center;
}
