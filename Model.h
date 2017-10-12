#pragma once
#include "Matrix.h"
#include "StateVector.h"
#include <vector>

#include "GLIncludes.h"

class Model {
    Matrix _model;
    StateVector _state;
    std::vector<Vector4> _vertices;
    GLuint _bufferContext;
public:
    Model();
    void render();

};