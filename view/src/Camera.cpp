#define _USE_MATH_DEFINES
#include "Camera.h"
#include "Logger.h"
#include "ShaderBroker.h"

Camera::Camera() {

    _debugShader             = static_cast<DebugShader*>(ShaderBroker::instance()->getShader("debugShader"));
    std::vector<Cube>* cubes = new std::vector<Cube>{Cube(2.0, 2.0, 2.0, Vector4(0.0, 0.0, 0.0))};
    _frustumVAO.createVAO(cubes, GeometryConstruction::TRIANGLE_MESH);

    _viewState = Camera::ViewState::DEFERRED_LIGHTING;

    // Used for god mode
    _state.setGravity(false);
}

Camera::~Camera() {}

void Camera::displayViewFrustum(Matrix view) {

    Vector4 color(1.0, 0.0, 0.0);

    // Model transform to create frustum cube
    MVP mvp;
    mvp.setModel(_mvp.getViewMatrix().inverse() * _mvp.getProjectionMatrix().inverse());
    mvp.setView(view); // set current view matrix to place frustum in correct location
    mvp.setProjection(_mvp.getProjectionMatrix());
    _debugShader->runShader(&mvp, &_frustumVAO, {}, color.getFlatBuffer(), GeometryConstruction::TRIANGLE_MESH);
}

void Camera::setProjection(Matrix projection) { _mvp.setProjection(projection); }

void Camera::setViewMatrix(Matrix transform) { _mvp.setView(transform); }

StateVector* Camera::getState() { return &_state; }

void Camera::setView(Matrix translation, Matrix rotation, Matrix scale) {
    Vector4 zero(0.f, 0.f, 0.f);
    _state.setLinearPosition(translation * zero);
    //_state.setAngularPosition(rotation * zero);
    // Debug to set camera to look over procedural geometry.
    _state.setAngularPosition(Vector4(43.0, 0.0, 0.0));
    _mvp.setView(_mvp.getViewMatrix() * scale * rotation * translation);
}

Matrix Camera::getProjection() { return _mvp.getProjectionMatrix(); }

Matrix Camera::getView() { return _mvp.getViewMatrix(); }

Camera::ViewState Camera::getViewState() { return _viewState; }

void Camera::updateState(int milliseconds) { _state.update(milliseconds); }

void Camera::setViewState(int key) {
    switch (key) {
        case GLFW_KEY_1: {
            _viewState = ViewState::DEFERRED_LIGHTING;
            break;
        }
        case GLFW_KEY_2: {
            _viewState = ViewState::DIFFUSE;
            break;
        }
        case GLFW_KEY_3: {
            _viewState = ViewState::NORMAL;
            break;
        }
        case GLFW_KEY_4: {
            _viewState = ViewState::POSITION;
            break;
        }
        case GLFW_KEY_5: {
            _viewState = ViewState::VELOCITY;
            break;
        }
        case GLFW_KEY_6: {
            _viewState = ViewState::SCREEN_SPACE_AMBIENT_OCCLUSION;
            break;
        }
        case GLFW_KEY_7: {
            _viewState = ViewState::CAMERA_SHADOW;
            break;
        }
        case GLFW_KEY_8: {
            _viewState = ViewState::MAP_SHADOW;
            break;
        }
        case GLFW_KEY_9: {
            _viewState = ViewState::POINT_SHADOW;
            break;
        }
        case GLFW_KEY_0: {
            _viewState = ViewState::PHYSICS;
            break;
        }
        default: { break; }
    };
}
