#pragma once
#include "Matrix.h"
#include "Vector4.h"
class ViewManager {

    Vector4 _pos;
    Matrix _view;
    Matrix _projection;

public:

    ViewManager() {}
    void applyTransform(Matrix transform);
};