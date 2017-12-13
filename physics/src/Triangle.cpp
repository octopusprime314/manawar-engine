#include "Triangle.h"

Triangle::Triangle(Vector4 A, Vector4 B, Vector4 C) {
   _points[0] = A;
   _points[1] = B;
   _points[2] = C;
}

Triangle::~Triangle() {

}

Vector4* Triangle::getTrianglePoints(){
    return _points;
}