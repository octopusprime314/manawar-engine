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

void Geometry::updatePosition(Vector4 position) {
    for (size_t i = 0; i < _spheres.size(); ++i) {
        _spheres[i].offsetPosition(position); //Adds an offset to the position of the sphere
    }

    ////TODO if necessary
    //for(int i = 0; i < _triangles.size(); ++i){
    //    _triangles[i].offsetPosition(position); //Adds an offset to the position of the sphere
    //}
}