#include "GeometryGraphic.h"

GeometryGraphic::GeometryGraphic(std::vector<Triangle>* triangles) {
    _vao.createVAO(triangles);
}

GeometryGraphic::GeometryGraphic(std::vector<Sphere>* spheres) {
    _vao.createVAO(spheres, GeometryConstruction::LINE_WIREFRAME);
}

GeometryGraphic::GeometryGraphic(std::vector<Cube>* cubes) {
    _vao.createVAO(cubes, GeometryConstruction::LINE_WIREFRAME);
}

GeometryGraphic::~GeometryGraphic() {

}

VAO* GeometryGraphic::getVAO() {
    return &_vao;
}

