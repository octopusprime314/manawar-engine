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
    void           _buildAnimationFrames(AnimatedModel* model, const std::vector<SkinningData>& skins);
    void           _buildTriangles(Model* model, std::vector<Vector4>& vertices, std::vector<Vector4>& normals, 
                   std::vector<Texture2>& textures, std::vector<int>& indices, Matrix translation, Matrix rotation, Matrix scale);
public:
    FbxLoader(std::string name);
    ~FbxLoader();
    FbxScene* getScene();
    void loadAnimatedModel(AnimatedModel* model, FbxNode* node = nullptr);
    void loadAnimatedModelData(AnimatedModel* model, FbxSkin* pSkin, FbxNode* node);
    void loadModel(Model* model, FbxNode* node = nullptr);
    void loadModelData(Model* model, FbxMesh* meshNode, FbxNode* childNode);
};