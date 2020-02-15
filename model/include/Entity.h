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
#include "EventSubscriber.h"
#include <iostream>
#include "MasterClock.h"
#include "MVP.h"
#include "AnimatedModel.h"

class IOEventDistributor;
class Model;
class FrustumCuller;

using VAOMap = std::map<int, std::vector<VAO*>>;

class Entity : public EventSubscriber {

public:

    //Default model to type to base class
    Entity(Model*      model,
           ViewEvents* eventWrapper,
           MVP         worldSpaceTransform = MVP());
    virtual ~Entity();

    void                        setRayTracingTextureId(unsigned int    rtId);
    void                        setLayeredTexture(     LayeredTexture* layeredTexture);
    void                        setPosition(           Vector4         position);
    void                        setVelocity(           Vector4         velocity);
    void                        setSelected(           bool            isSelected);
    bool                        isID(                  unsigned int    entityID);

    Matrix                      getWorldSpaceTransform();
    unsigned int                getRayTracingTextureId();
    LayeredTexture*             getLayeredTexture();
    FrustumCuller*              getFrustumCuller();
    std::vector<RenderBuffers>* getRenderBuffers();
    StateVector*                getStateVector();
    std::vector<VAO*>*          getFrustumVAO();
    VAOMap                      getVAOMapping();
    void                        setMVP(MVP mvp);
    bool                        getSelected();
    Geometry*                   getGeometry();
    MVP*                        getPrevMVP();
    Model*                      getModel();
    MVP*                        getMVP();
    unsigned int                getID();

protected:
    std::vector<RenderBuffers>* _frustumRenderBuffers;
    unsigned int                _rayTracingTextureId;
    Matrix                      _worldSpaceTransform;
    //Entity space geometry
    Geometry                    _worldSpaceGeometry;
    VAOMap                      _frustumVAOMapping;
    LayeredTexture*             _layeredTexture;
    FrustumCuller*              _frustumCuller;
    std::vector<VAO*>           _frustumVAOs;
    //id generator that is incremented every time a new Entity is added
    static unsigned int         _idGenerator;
    EngineStateFlags            _gameState;
    bool                        _selected;
    //Previous Model view matrix container for motion blur
    MVP                         _prevMVP;
    StateVector                 _state;
    MasterClock*                _clock;
    Model*                      _model;
    MVP                         _mvp;
    unsigned int                _id;

    void                        _updateReleaseKeyboard(int              key,
                                                       int              x,
                                                       int              y) {};
    void                        _updateGameState(      EngineStateFlags state);
    void                        _updateKeyboard(       int              key,
                                                       int              x,
                                                       int              y) {};
    void                        _updateMouse(          double           x,
                                                       double           y) {};
    void                        _updateProjection(     Matrix           projection);
    void                        _updateKinematics(     int              milliSeconds);
    void                        _updateAnimation(      int              milliSeconds);
    void                        _updateView(           Matrix           view);
    void                        _generateVAOTiles();
    void                        _updateDraw();
};