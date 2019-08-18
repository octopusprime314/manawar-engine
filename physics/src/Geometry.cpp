#include "Geometry.h"

Geometry::Geometry() {

}

Geometry::~Geometry() {

}

void Geometry::addTriangle(Triangle triangle) {
    _triangles.push_back(triangle);
}

void Geometry::addSphere(Sphere sphere) {
    _spheres.push_back(sphere);
}

std::vector<Triangle>* Geometry::getTriangles() {
    return &_triangles;
}

std::vector<Sphere>* Geometry::getSpheres() {
    return &_spheres;
}

void Geometry::updateTransform(Matrix transform) {
    for (size_t i = 0; i < _spheres.size(); ++i) {
        //Adds an offset to the position of the sphere
        _spheres[i].offsetTransform(transform);
    }

    ////TODO if necessary
    //for(int i = 0; i < _triangles.size(); ++i){
    //    //Adds an offset to the position of the sphere
    //    _triangles[i].offsetPosition(position);
    //}
}
