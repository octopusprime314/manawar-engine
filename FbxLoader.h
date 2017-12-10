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
#include "Texture2.h"

class Model;
class AnimatedModel;
class SkinningData;
class Vector4;
class Matrix;

class FbxLoader {
    FbxManager*    _fbxManager;
    FbxIOSettings* _ioSettings;
    FbxScene*      _scene;
    void           _loadTextures(Model* model, FbxMesh* meshNode, FbxNode* childNode);
    void           _buildTriangles(Model* model, std::vector<Vector4>& vertices, std::vector<Vector4>& normals, 
                        std::vector<Texture2>& textures, std::vector<int>& indices, FbxNode* node);
    void           _generateTextureStrides(FbxMesh* meshNode, std::vector<int>& textureStrides);
    bool           _loadTexture(Model* model, int textureStride, FbxFileTexture* textureFbx, int textureIndex);
    void           _loadTextureUVs(FbxMesh* meshNode, std::vector<Texture2>& textures);
    void           _loadNormals(FbxMesh* meshNode, int* indices, std::vector<Vector4>& normals);
    void           _loadVertices(FbxMesh* meshNode, std::vector<Vector4>& vertices);
    void           _loadIndices(Model* model, FbxMesh* meshNode, int*& indices);
    void           _buildModelData(Model* model, FbxMesh* meshNode, FbxNode* childNode, std::vector<Vector4>& vertices, 
                        std::vector<Vector4>& normals, std::vector<Texture2>& textures);
public:
    FbxLoader(std::string name);
    ~FbxLoader();
    FbxScene* getScene();
    void loadAnimatedModel(AnimatedModel* model, FbxNode* node = nullptr);
    void loadAnimatedModelData(AnimatedModel* model, FbxSkin* pSkin, FbxNode* node, FbxMesh* mesh);
    void loadModel(Model* model, FbxNode* node = nullptr);
    void loadModelData(Model* model, FbxMesh* meshNode, FbxNode* childNode);
    void buildAnimationFrames(AnimatedModel* model, std::vector<SkinningData>& skins);
};