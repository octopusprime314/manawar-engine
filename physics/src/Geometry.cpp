#include "Geometry.h"

Geometry::Geometry() {
   
}

Geometry::~Geometry() {

}

void Geometry::addTriangle(Triangle triangle){
    _triangles.push_back(triangle);
}

void Geometry::addSphere(Sphere sphere){
    _spheres.push_back(sphere);
}
