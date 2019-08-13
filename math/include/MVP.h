/*
* MVP is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
* Copyright (c) 2017 Peter Morley.
*
* ReBoot is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, version 3.
*
* ReBoot is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/**
*  MVP class. Model View Projection matrix container
*/

#pragma once
#include "Matrix.h"

class MVP {
    //Projection matrix based on ViewEventDistributor/Camera
    Matrix _projection;
    //Normal matrix based on inverse transpose matrix of the ViewEventDistributor/Camera matrix
    Matrix _normal;
    //Object and World Space Matrix i.e. how the model is centered around the origin and
    //where it is placed in the context of the game World
    Matrix _model;
    //View matrix updates from ViewEventDistributor/Camera
    Matrix _view;
public:
    MVP();
    ~MVP();
    void   setProjection(Matrix projection);
    void   setNormal(    Matrix normal);
    void   setModel(     Matrix model);
    void   setView(      Matrix view);
    Matrix getProjectionMatrix();
    float* getProjectionBuffer();
    Matrix getNormalMatrix();
    float* getNormalBuffer();
    float* getModelBuffer();
    Matrix getModelMatrix();
    Matrix getViewMatrix();
    float* getViewBuffer();


};