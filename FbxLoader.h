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