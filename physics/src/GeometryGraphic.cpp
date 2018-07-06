#include "GeometryGraphic.h"

GeometryGraphic::GeometryGraphic(std::vector<Triangle>* triangles) {
    _vao.createVAO(triangles);
}

GeometryGraphic::GeometryGraphic(std::vector<Sphere>* spheres) {
    _vao.createVAO(spheres);
}

GeometryGraphic::~GeometryGraphic() {

}

VAO* GeometryGraphic::getVAO() {
    return &_vao;
}

