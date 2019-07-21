/*
* FbxLoader is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  FbxLoader class. Loads and converts fbx data into ReBoot engine format
*/

#pragma once
#include <string>
#include "fbxsdk.h"
#include <vector>
#include "Tex2.h"
#include <map>
#include "Matrix.h"

class Model;
class AnimatedModel;
class SkinningData;
class Vector4;
class Entity;
class RenderBuffers;

struct FbxExporterType {
    FbxManager*     manager;
    FbxIOSettings*  ioSettings;
    FbxScene*       scene;
    FbxExporter*    exporter;
};

class FbxLoader {

    using TileTextures   = std::map<std::string, std::vector<std::string>>;
    using ClonedCount    = std::map<std::string, unsigned int>;
    using ClonedMatrices = std::map<std::string, Matrix>;
    using TextureStrides = std::vector<std::pair<int, int>>;
    
    ClonedMatrices  _clonedWorldTransforms;
    Matrix          _objectSpaceTransform;
    ClonedCount     _clonedInstances;
    bool            _copiedOverFlag;
    TileTextures    _tileTextures;
    int             _strideIndex;
    FbxManager*     _fbxManager;
    FbxIOSettings*  _ioSettings;
    std::string     _fileName;
    FbxScene*       _scene;
    FbxExporterType _export;

    void            _loadTextures(          Model* model,
                                            FbxMesh* meshNode,
                                            FbxNode* childNode);
                                            
    void            _buildTriangles(        Model* model,
                                            std::vector<Vector4>& vertices,
                                            std::vector<Vector4>& normals,
                                            std::vector<Tex2>& textures,
                                            std::vector<int>& indices,
                                            FbxNode* node);
                                            
    void            _buildModelData(        Model* model,
                                            FbxMesh* meshNode,
                                            FbxNode* childNode,
                                            std::vector<Vector4>& vertices,
                                            std::vector<Vector4>& normals,
                                            std::vector<Tex2>& textures);
                                            
    void            _buildGeometryData(     Model* model,
                                            std::vector<Vector4>& vertices,
                                            std::vector<int>& indices,
                                            FbxNode* node);
                                            
    void            _nodeExists(            std::string modelName,
                                            FbxNode* node,
                                            std::vector<FbxNode*>& nodes);

    void            _generateTextureStrides(FbxMesh* meshNode,
                                            TextureStrides& textureStrides);

    bool            _loadTexture(           Model* model,
                                            int textureStride,
                                            FbxFileTexture* textureFbx);

    bool            _loadLayeredTexture(    Model* model,
                                            int textureStride,
                                            FbxLayeredTexture* layered_texture);
                                            
    void            _loadTextureUVs(        FbxMesh* meshNode,
                                            std::vector<Tex2>& textures);
                                            
    void            _loadNormals(           FbxMesh* meshNode,
                                            int* indices,
                                            std::vector<Vector4>& normals);
                                            
    void            _loadVertices(          FbxMesh* meshNode,
                                            std::vector<Vector4>& vertices);
                                            
    void            _loadIndices(           Model* model,
                                            FbxMesh* meshNode,
                                            int*& indices);
                                            
    void            _cloneFbxNode(          std::string modelName,
                                            FbxLoader* fbxToAdd,
                                            Vector4 location,
                                            Vector4 rotation);
                                            
    void            _searchAndEditNode(     FbxNode* rootNode,
                                            FbxNode* childNode,
                                            std::string modelName,
                                            Vector4 location,
                                            Vector4 rotation);
                                            
    void            _searchAndEditMesh(     FbxNode* childNode,
                                            std::string modelName,
                                            Vector4 location,
                                            Vector4 rotation);

    int             _getASCIIFormatIndex(   FbxManager* fbxManager);

    void            _parseTags(             FbxNode* node);


public:
    FbxLoader(std::string name);
    ~FbxLoader();
    void            loadAnimatedModel(      AnimatedModel* model,
                                            FbxNode* node = nullptr);

    void            loadAnimatedModelData(  AnimatedModel* model,
                                            FbxSkin* pSkin,
                                            FbxNode* node,
                                            FbxMesh* mesh);
                                            
    void            loadModel(              Model* model,
                                            FbxNode* node = nullptr);
                                            
    void            loadModelData(          Model* model,
                                            FbxMesh* meshNode,
                                            FbxNode* childNode);
                                            
    void            buildAnimationFrames(   AnimatedModel* model,
                                            std::vector<SkinningData>& skins);
                                            
    void            loadGeometryData(       Model* model,
                                            FbxMesh* meshNode,
                                            FbxNode* childNode);

    void            addToScene(             Model* modelAddedTo,
                                            FbxLoader* modelToLoad, 
                                            Vector4 location,
                                            Vector4 rotation);

    void            removeFromScene(        Entity* entityToRemove,
                                            FbxLoader* modelRemovedFrom, 
                                            std::vector<FbxNode*>& nodesToRemove,
                                            FbxNode* node = nullptr);

    void            addTileToScene(         Model* modelAddedTo,
                                            FbxLoader* modelToLoad, 
                                            Vector4 location,
                                            std::vector<std::string> textures);

    void            loadGeometry(           Model* model,
                                            FbxNode* node);

    void            buildGfxAABB(           Model* model,
                                            Matrix scale);

    void            buildCollisionAABB(     Model* model);

    Matrix          getObjectSpaceTransform();
    std::string     getModelName();
    void            clearScene();
    void            saveScene();
    FbxScene*       getScene();

};