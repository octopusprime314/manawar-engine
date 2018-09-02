#include "ModelBroker.h"
#include "Dirent.h"
#include <algorithm>
#include <cctype>
#include "AnimatedModel.h"
#include "Model.h"
#include "Logger.h"
ModelBroker* ModelBroker::_broker = nullptr;
ViewEventDistributor* ModelBroker::_viewManager = nullptr;

ModelBroker* ModelBroker::instance() { //Only initializes the static pointer once
    if (_broker == nullptr) {
        _broker = new ModelBroker();
    }
    return _broker;
}
ModelBroker::ModelBroker() {

}
ModelBroker::~ModelBroker() {

}

//helper function to capitalize everything
std::string ModelBroker::_strToUpper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return std::toupper(c); } // correct
    );
    return s;
}

//helper function to capitalize everything
std::string ModelBroker::_strToLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return std::tolower(c); } // correct
    );
    return s;
}

void ModelBroker::setViewManager(ViewEventDistributor* viewEventDistributor) {
    _viewManager = viewEventDistributor;
}

ViewEventDistributor* ModelBroker::getViewManager() {
    return _viewManager;
}

void ModelBroker::buildModels() {

    _gatherModelNames();
}

Model* ModelBroker::getModel(std::string modelName) {
    std::string upperCaseMapName = _strToUpper(modelName);
    return _models[upperCaseMapName];
}

void ModelBroker::clearChanges(std::string modelName) {

    std::string upperCaseMapName = _strToUpper(modelName + "/" + modelName + ".fbx");

    if (_models.find(upperCaseMapName) != _models.end()) {

        FbxLoader* fbxScene = _models[upperCaseMapName]->getFbxLoader();
        fbxScene->clearScene();
    }
    else {
        std::cout << "Model doesn't exist so add it!" << std::endl;
    }
}

void ModelBroker::addModel(std::string modelName, std::string modelToAdd, Vector4 location) {
    
    std::string upperCaseMapName = _strToUpper(modelName + "/" + modelName + ".fbx");
    std::string upperCaseMapNameToAdd = _strToUpper(modelToAdd + "/" + modelToAdd + ".fbx");

    if (_models.find(upperCaseMapName) != _models.end() &&
        _models.find(upperCaseMapNameToAdd) != _models.end()) {

        FbxLoader* modelToAdd = _models[upperCaseMapNameToAdd]->getFbxLoader();
        FbxLoader* modelAddedTo = _models[upperCaseMapName]->getFbxLoader();
        modelAddedTo->addToScene(_models[upperCaseMapName], modelToAdd, location);
    }
    else {
        std::cout << "Model doesn't exist so add it!" << std::endl;
    }
}

void ModelBroker::addTileModel(std::string modelName, 
    std::string modelToAdd, 
    Vector4 location, 
    std::vector<std::string> textures) {

    std::string upperCaseMapName = _strToUpper(modelName + "/" + modelName + ".fbx");
    std::string upperCaseMapNameToAdd = _strToUpper(modelToAdd + "/" + modelToAdd + ".fbx");

    if (_models.find(upperCaseMapName) != _models.end() &&
        _models.find(upperCaseMapNameToAdd) != _models.end()) {

        FbxLoader* modelToAdd = _models[upperCaseMapNameToAdd]->getFbxLoader();
        FbxLoader* modelAddedTo = _models[upperCaseMapName]->getFbxLoader();
        for (auto& texture : textures) {
            texture = _strToLower(texture);
        }
        modelAddedTo->addTileToScene(_models[upperCaseMapName], modelToAdd, location, textures);
    }
    else {
        std::cout << "Model doesn't exist so add it!" << std::endl;
    }
}

void ModelBroker::saveModel(std::string modelName) {

    std::string upperCaseMapName = _strToUpper(modelName + "/" + modelName + ".fbx");

    if (_models.find(upperCaseMapName) != _models.end()) {

        FbxLoader* modelToSave = _models[upperCaseMapName]->getFbxLoader();
        modelToSave->saveScene();
    }
    else {
        std::cout << "Model doesn't exist so don't save it!" << std::endl;
    }
}

void ModelBroker::updateModel(std::string modelName) {

    std::string upperCaseMapName = _strToUpper(modelName + "/" + modelName + ".fbx");

    if (_models.find(upperCaseMapName) != _models.end()) {

        if (_models[upperCaseMapName]->getClassType() == ModelClass::ModelType) {
            auto model = new Model(upperCaseMapName);
            _models[upperCaseMapName]->updateModel(model);
            delete model;
        }
        else if (_models[upperCaseMapName]->getClassType() == ModelClass::AnimatedModelType) {
            auto model = new AnimatedModel(upperCaseMapName);
            _models[upperCaseMapName]->updateModel(model);
            delete model;
        }

    }
    else {
        std::cout << "Model doesn't exist so add it!" << std::endl;
    }
}


void ModelBroker::_gatherModelNames()
{
    bool isFile = false;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(STATIC_MESH_LOCATION.c_str())) != nullptr)
    {
        Logger::INFO("Files to be processed: \n");

        while ((ent = readdir(dir)) != nullptr) {
            if (*ent->d_name) {
                std::string fileName = std::string(ent->d_name);
           
                if (!fileName.empty() && 
                    fileName != "." && 
                    fileName != ".." && 
                    fileName.find(".ini") == std::string::npos) {
                    
                    Logger::INFO(ent->d_name, "\n");

                    std::string mapName = fileName + "/" + fileName + ".fbx";
                    std::string upperCaseMapName = _strToUpper(mapName);
                    _models[upperCaseMapName] = new Model(mapName);
                }
            }
        }
        closedir(dir);
    }
    else
    {
        std::cout << "Problem reading from directory!" << std::endl;
    }

    if ((dir = opendir(ANIMATED_MESH_LOCATION.c_str())) != nullptr)
    {
        Logger::INFO("Files to be processed: \n");

        while ((ent = readdir(dir)) != nullptr) {
            if (*ent->d_name) {
                std::string fileName = std::string(ent->d_name);

                if (!fileName.empty() &&
                    fileName != "." &&
                    fileName != ".." &&
                    fileName.find(".ini") == std::string::npos) {

                    Logger::INFO(ent->d_name, "\n");

                    std::string mapName = fileName + "/" + fileName + ".fbx";
                    std::string upperCaseMapName = _strToUpper(mapName);
                    _models[upperCaseMapName] = static_cast<Model*>(new AnimatedModel(mapName));
                }
            }
        }
        closedir(dir);
    }
    else
    {
        std::cout << "Problem reading from directory!" << std::endl;
    }
}

