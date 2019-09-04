/*
* Model is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  Model class. Contains vertex and normal data which will get populated in opengl
*  buffers to be rendered to the screen
*/

#pragma once
#include "Matrix.h"
#include "Tex2.h"
#include "StateVector.h"
#include <vector>
#include "GLIncludes.h"
#include "StaticShader.h"
#include "DebugShader.h"
#include <iostream>
#include "FbxLoader.h"
#include "MasterClock.h"
#include "TextureBroker.h"
#include "Geometry.h"
#include "VAO.h"
#include "MVP.h"
#include "RenderBuffers.h"
#include "ForwardShader.h"
#include <mutex>
class FrustumCuller;
class IOEventDistributor;

enum class ModelClass {
    ModelType = 0,
    AnimatedModelType
};

class Model {

public:
    //Default model to type to base class
    Model(std::string name,
          ModelClass  classId = ModelClass::ModelType);

    virtual ~Model();
    void                        addLayeredTexture(  std::vector<std::string> textureNames,
                                                    int                      stride);
    void                        addTexture(         std::string textureName,
                                                    int         stride);
    void                        setInstances(       std::vector<Vector4> offsets);
    LayeredTexture*             getLayeredTexture(  std::string textureName);
    AssetTexture*               getTexture(         std::string textureName);
    void                        addVAO(             ModelClass classType);
    void                        addGeometryTriangle(Triangle triangle);
    void                        runShader(          Entity* entity);
    void                        setAABB(            Cube* aabbCube);
    void                        addGeometrySphere(  Sphere sphere);
    void                        setGfxAABB(         Cube* gfxAABB);
    virtual void                updateModel(        Model* model);
    bool                        getIsInstancedModel();
    float*                      getInstanceOffsets();
    RenderBuffers*              getRenderBuffers();
    std::vector<std::string>    getTextureNames();
    GeometryType                getGeometryType();
    size_t                      getArrayCount();
    ModelClass                  getClassType();
    FbxLoader*                  getFbxLoader();
    Geometry*                   getGeometry();
    Cube*                       getGfxAABB();
    Cube*                       getAABB();
    std::string                 getName();
    std::vector<VAO*>*          getVAO();
    unsigned int                getId();

protected:
    std::string                 _getModelName(std::string name);

    std::vector<std::string>    _textureRecorder;
    //Static texture manager for texture reuse purposes, all models have access
    static TextureBroker*       _textureManager;
    static unsigned int         _modelIdTagger;
    //Manages vertex, normal and texture data
    RenderBuffers               _renderBuffers;
    //Container object of the Model's shader
    StaticShader*               _shaderProgram;
    //Indicates whether the collision geometry is sphere or triangle based
    GeometryType                _geometryType;
    //300 x, y and z offsets
    float                       _offsets[900];
    bool                        _isInstanced;
    std::mutex                  _updateLock;

    int                         _instances;
    //Used to load fbx data and parse it into engine format
    FbxLoader*                  _fbxLoader;
    Cube*                       _aabbCube;
    //Geometry object that contains all collision information for a model
    Geometry                    _geometry;
    //Used to identify which class is being used
    ModelClass                  _classId;
    Cube*                       _gfxAABB;
    //used to identify model, used for ray tracing
    unsigned int                _modelId;
    std::string                 _name;
    //Vao container
    std::vector<VAO*>           _vao;
};