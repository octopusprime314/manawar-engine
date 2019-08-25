/*
 * Camera is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
 *  Camera class.  Has an MVP matrix that places the orientation and type of camera
 *  which could include ortho/perspective projections.  Maintains a state vector for smooth motion.
 */

#pragma once
#include "DebugShader.h"
#include "MVP.h"
#include "Matrix.h"
#include "StateVector.h"

class Camera {
  public:
    enum class ViewState {
        DEFERRED_LIGHTING = 0,
        DIFFUSE,
        NORMAL,
        POSITION,
        VELOCITY,
        SCREEN_SPACE_AMBIENT_OCCLUSION,
        CAMERA_SHADOW,
        MAP_SHADOW,
        POINT_SHADOW,
        PHYSICS
    };

  private:
    ViewState    _viewState;
    MVP          _mvp;
    StateVector  _state;
    VAO          _frustumVAO;
    DebugShader* _debugShader;

  public:
    Camera();
    ~Camera();
    void         setProjection(Matrix projection);
    void         setView(Matrix translation, Matrix rotation, Matrix scale);
    void         setViewMatrix(Matrix transform);
    Matrix       getProjection();
    Matrix       getView();
    ViewState    getViewState();
    void         displayViewFrustum(Matrix view);
    void         setViewState(int key);
    virtual void updateState(int milliseconds);
    StateVector* getState();
};
