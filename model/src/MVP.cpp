#include "MVP.h"

MVP::MVP() {

}
MVP::~MVP(){

}

float* MVP::getModelBuffer() {
    return _model.getFlatBuffer();
}

float* MVP::getViewBuffer() {
    return _view.getFlatBuffer();
}

float* MVP::getProjectionBuffer() {
    return _projection.getFlatBuffer();
}

float* MVP::getNormalBuffer() {
    return _normal.getFlatBuffer();
}

Matrix MVP::getModelMatrix() {
    return _model;
}

Matrix MVP::getViewMatrix() {
    return _view;
}

Matrix MVP::getProjectionMatrix() {
    return _projection;
}

Matrix MVP::getNormalMatrix() {
    return _normal;
}

void MVP::setModel(Matrix model) {
    _model = model;
}

void MVP::setView(Matrix view) {
    _view = view;
}

void MVP::setProjection(Matrix projection) {
    _projection = projection;
}

void MVP::setNormal(Matrix normal) {
    _normal = normal;
}
