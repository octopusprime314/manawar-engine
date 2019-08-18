#include "Triangle.h"

Triangle::Triangle(Vector4 A,
                   Vector4 B,
                   Vector4 C)
    : _points{ A,
               B,
               C }
{
}

Triangle::~Triangle() {

}

Vector4x3 Triangle::getTrianglePoints() {
    return _points;
}
