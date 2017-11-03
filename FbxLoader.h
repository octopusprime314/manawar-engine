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

class Model;
class AnimatedModel;
const float DEG_2_RAD = 3.14159265f/180.0f;

class FbxLoader {
    FbxManager*    _fbxManager;
    FbxIOSettings* _ioSettings;
    FbxScene*      _scene;
public:
    FbxLoader(std::string name);
    ~FbxLoader();
    FbxScene* getScene();
	void loadAnimatedModel(AnimatedModel* model, FbxNode* node = nullptr);
	void loadAnimatedModelData(AnimatedModel* model, FbxSkin* pSkin, FbxNode* node);
    void loadModel(Model* model, FbxNode* node = nullptr);
    void loadModelData(Model* model, FbxMesh* meshNode, FbxNode* childNode);
};