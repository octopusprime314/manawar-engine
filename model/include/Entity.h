/*
* Entity is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  Entity class. Contains a model class with all of the render information and various 
*  physics/kinematic data about how the model moves in the world
*/

#pragma once
#include "StateVector.h"
#include <vector>
#include "UpdateInterface.h"
#include <iostream>
#include "MasterClock.h"
#include "MVP.h"
#include "AnimatedModel.h"

class SimpleContext;
class Model;
class FrustumCuller;

class Entity : public UpdateInterface {

    using VAOMap = std::map<int, std::vector<VAO*>>;
public:

    //Default model to type to base class
    Entity(Model* model, ViewManagerEvents* eventWrapper);
    virtual ~Entity();
    Model*                      getModel();
    MVP*                        getMVP();
    MVP*                        getPrevMVP();
    StateVector*                getStateVector();
    void                        setPosition(Vector4 position);
    void                        setVelocity(Vector4 velocity);
    std::vector<VAO*>*          getFrustumVAO();
    FrustumCuller*              getFrustumCuller();
protected:

    VAOMap                      _frustumVAOMapping;
    std::vector<VAO*>           _frustumVAOs; //vaos that are in view for entity
    FrustumCuller*              _frustumCuller;
    StateVector                 _state; //Kinematics
    MasterClock*                _clock; //Used to coordinate time with the world
    Model*                      _model; //Graphics data
    MVP                         _mvp; //Model view matrix container
    MVP                         _prevMVP; //Previous Model view matrix container for motion blur

    void                        _generateVAOTiles();
    void                        _updateKeyboard(int key, int x, int y) {}; //Do stuff based on keyboard upate
    void                        _updateReleaseKeyboard(int key, int x, int y) {};
    void                        _updateMouse(double x, double y) {}; //Do stuff based on mouse update
    void                        _updateDraw(); //Do draw stuff
    void                        _updateGameState(int state);
    void                        _updateView(Matrix view); //Get view matrix updates
    void                        _updateProjection(Matrix projection); //Get projection matrix updates
    void                        _updateKinematics(int milliSeconds);
    void                        _updateAnimation(int milliSeconds);
};