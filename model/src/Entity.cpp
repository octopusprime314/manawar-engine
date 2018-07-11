#include "Entity.h"
#include "SimpleContext.h"
#include "Model.h"

Entity::Entity(Model* model, ViewManagerEvents* eventWrapper) :
    UpdateInterface(eventWrapper),
    _clock(MasterClock::instance()),
    _model(model) {

    if (_model->getClassType() == ModelClass::AnimatedModelType) {
        _state.setActive(true); //initialize animations to be active
        
        //Hook up to framerate update for proper animation progression
        _clock->subscribeAnimationRate(std::bind(&Entity::_updateAnimation, this, std::placeholders::_1));
    }
    //Hook up to kinematic update for proper physics handling
    _clock->subscribeKinematicsRate(std::bind(&Entity::_updateKinematics, this, std::placeholders::_1));

}

Entity::~Entity() {
}

void Entity::_updateDraw() {

    if (_model->getClassType() == ModelClass::AnimatedModelType) {

        AnimatedModel* animatedModel = static_cast<AnimatedModel*>(_model);
        animatedModel->updateAnimation();
    }

    //Run model shader by allowing the shader to operate on the model
    _model->runShader(this);
}

Model* Entity::getModel() {
    return _model;
}

void Entity::_updateAnimation(int milliSeconds) {
    //Coordinate loading new animation frame to gpu
    if (_model->getClassType() == ModelClass::AnimatedModelType) {

        AnimatedModel* animatedModel = static_cast<AnimatedModel*>(_model);
        animatedModel->triggerNextFrame();
    }
}

void Entity::_updateView(Matrix view) {

    _prevMVP.setView(_mvp.getViewMatrix());

    _mvp.setView(view); //Receive updates when the view matrix has changed

    //If view changes then change our normal matrix
    _mvp.setNormal(view.inverse().transpose());
}

void Entity::_updateProjection(Matrix projection) {
    _mvp.setProjection(projection); //Receive updates when the projection matrix has changed
}

void Entity::_updateKinematics(int milliSeconds) {
    //Do kinematic calculations
    _state.update(milliSeconds);

    _prevMVP.setModel(_mvp.getModelMatrix());

    //Pass position information to model matrix
    Vector4 position = _state.getLinearPosition();
    _model->getGeometry()->updatePosition(position);
    _mvp.getModelBuffer()[3] = position.getx();
    _mvp.getModelBuffer()[7] = position.gety();
    _mvp.getModelBuffer()[11] = position.getz();
}


MVP* Entity::getMVP() {
    return &_mvp;
}
MVP* Entity::getPrevMVP() {
    return &_prevMVP;
}

StateVector* Entity::getStateVector() {
    return &_state;
}

void Entity::setPosition(Vector4 position) {
    _state.setLinearPosition(position);
    //Pass position information to model matrix
    _model->getGeometry()->updatePosition(position);

    _prevMVP.setModel(_mvp.getModelMatrix());

    _mvp.setModel(Matrix::translation(position.getx(), position.gety(), position.getz()));
}

void Entity::setVelocity(Vector4 velocity) {
    _state.setLinearVelocity(velocity);
}
