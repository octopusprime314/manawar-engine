#include "AnimationFrame.h"

AnimationFrame::AnimationFrame(std::vector<Vector4>* vertices, std::vector<Vector4>* normals, std::vector<Vector4>* debugNormals) {
    _vertices = vertices;
    _normals = normals;
    _debugNormals = debugNormals;
}

AnimationFrame::~AnimationFrame() {
    delete _vertices;
    delete _normals;
    delete _debugNormals;
}

std::vector<Vector4>* AnimationFrame::getVertices() {
    return _vertices;
}

std::vector<Vector4>* AnimationFrame::getNormals() {
    return _normals;
}

std::vector<Vector4>* AnimationFrame::getDebugNormals() {
    return _debugNormals;
}
