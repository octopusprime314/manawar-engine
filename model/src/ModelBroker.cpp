#include "ModelBroker.h"
#include "Dirent.h"
#include <algorithm>
#include <cctype>
#include "AnimatedModel.h"
#include "Model.h"
#include "Logger.h"
#include "ViewEventDistributor.h"

ModelBroker*          ModelBroker::_broker      = nullptr;
ViewEventDistributor* ModelBroker::_viewManager = nullptr;

ModelBroker* ModelBroker::instance() {
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
    std::transform(s.begin(),
                   s.end(),
                   s.begin(),
                   [](unsigned char c) { return std::toupper(c); }
    );
    return s;
}

//helper function to capitalize everything
std::string ModelBroker::_strToLower(std::string s) {
    std::transform(s.begin(),
                   s.end(),
                   s.begin(),
                   [](unsigned char c) { return std::tolower(c); }
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
    if (_models.find(upperCaseMapName + "_LOD1") != _models.end()) {
        return _models[upperCaseMapName + "_LOD1"];
    }
    else {
        return _models[upperCaseMapName];
    }
}

Model* ModelBroker::getModel(std::string modelName, Vector4 pos) {
    std::string upperCaseMapName = _strToUpper(modelName);
    //If we have an lod marker from the model folder then choose which one
    if (_models.find(upperCaseMapName) != _models.end() &&
        upperCaseMapName.find("LOD") != std::string::npos) {

        Vector4 cameraPos = getViewManager()->getCameraPos();
        float distance    = (pos - cameraPos).getMagnitude();
        //use lod 1 which is the highest poly count for the model
        if (distance < 400 ||
            EngineState::getEngineState().worldEditorModeEnabled) {
             return _models[upperCaseMapName];
        }
        else {
            std::string lod = upperCaseMapName.substr(0, upperCaseMapName.size() - 1);
            if (_models.find(lod + "2") != _models.end()) {
                return _models[lod + "2"];
            }
            if (_models.find(lod + "3") != _models.end()) {
                return _models[lod + "3"];
            }
            else {
                return _models[upperCaseMapName];
            }
        }
    }
    else {
        return _models[upperCaseMapName];
    }
}

std::vector<std::string> ModelBroker::getModelNames() {
    return _modelNames;
}

void ModelBroker::clearChanges(std::string modelName) {

    std::string upperCaseMapName = _strToUpper(modelName);
    if (_models.find(upperCaseMapName) != _models.end()) {

        FbxLoader* fbxScene      = _models[upperCaseMapName]->getFbxLoader();
        fbxScene->clearScene();
    }
    else {
        std::cout << "Model doesn't exist so add it!" << std::endl;
    }
}

void ModelBroker::addModel(std::string modelName, std::string modelToAdd, Vector4 location, Vector4 rotation) {
    
    std::string upperCaseMapName      = _strToUpper(modelName);
    std::string upperCaseMapNameToAdd = _strToUpper(modelToAdd) + "_LOD1";

    if (_models.find(upperCaseMapName)      != _models.end() &&
        _models.find(upperCaseMapNameToAdd) != _models.end()) {

        FbxLoader* modelToAdd   = _models[upperCaseMapNameToAdd]->getFbxLoader();
        FbxLoader* modelAddedTo = _models[upperCaseMapName]->getFbxLoader();
        modelAddedTo->addToScene(_models[upperCaseMapName],
                                 modelToAdd,
                                 location,
                                 rotation);
    }
    else {
        std::cout << "Model doesn't exist so add it!" << std::endl;
    }
}

void ModelBroker::removeModel(Entity* entityToRemove, std::string modelRemovedFrom) {

    std::string upperCaseMapName    = _strToUpper(modelRemovedFrom);

    if (_models.find(upperCaseMapName) != _models.end() &&
        entityToRemove                 != nullptr) {

        FbxLoader* modelRemovedFrom = _models[upperCaseMapName]->getFbxLoader();
        modelRemovedFrom->removeFromScene(entityToRemove, modelRemovedFrom, std::vector<FbxNode*>());
    }
    else {
        std::cout << "Model doesn't exist so add it!" << std::endl;
    }
}

void ModelBroker::addTileModel(std::string              modelName,
                               std::string              modelToAdd,
                               Vector4                  location,
                               std::vector<std::string> textures) {

    std::string upperCaseMapName      = _strToUpper(modelName);
    std::string upperCaseMapNameToAdd = _strToUpper(modelToAdd) + "_LOD1";

    if (_models.find(upperCaseMapName)      != _models.end() &&
        _models.find(upperCaseMapNameToAdd) != _models.end()) {

        FbxLoader* modelToAdd   = _models[upperCaseMapNameToAdd]->getFbxLoader();
        FbxLoader* modelAddedTo = _models[upperCaseMapName]->getFbxLoader();
        for (auto& texture : textures) {
            texture = _strToLower(texture);
        }
        modelAddedTo->addTileToScene(_models[upperCaseMapName],
                                     modelToAdd,
                                     location,
                                     textures);
    }
    else {
        std::cout << "Model doesn't exist so add it!" << std::endl;
    }
}

void ModelBroker::saveModel(std::string modelName) {

    std::string upperCaseMapName = _strToUpper(modelName);

    if (_models.find(upperCaseMapName) != _models.end()) {

        FbxLoader* modelToSave   = _models[upperCaseMapName]->getFbxLoader();
        modelToSave->saveScene();
    }
    else {
        std::cout << "Model doesn't exist so don't save it!" << std::endl;
    }
}

void ModelBroker::updateModel(std::string modelName) {

    std::string upperCaseMapName = _strToUpper(modelName);

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


void ModelBroker::_gatherModelNames() {
    bool           isFile = false;
    DIR*           dir;
    struct dirent* ent;
    if ((dir = opendir(STATIC_MESH_LOCATION.c_str())) != nullptr) {
        LOG_INFO("Files to be processed: \n");

        while ((ent = readdir(dir)) != nullptr) {
            if (*ent->d_name) {
                std::string fileName = std::string(ent->d_name);
           
                if (fileName.empty()      == false &&
                    fileName              != "."   &&
                    fileName              != ".."  &&
                    fileName.find(".ini") == std::string::npos) {

                    DIR*           subDir;
                    struct dirent* subEnt;
                    if ((subDir = opendir((STATIC_MESH_LOCATION + fileName).c_str())) != nullptr) {
                        while ((subEnt = readdir(subDir)) != nullptr) {

                            if (*subEnt->d_name) {
                                LOG_INFO(subEnt->d_name, "\n");

                                std::string lodFile               = std::string(subEnt->d_name);
                                if (lodFile.find("lod") != std::string::npos) {
                                    std::string mapName           = fileName + "/" + lodFile;
                                    lodFile                       = lodFile.substr(0, lodFile.size() - 4);
                                    _models[_strToUpper(lodFile)] = new Model(STATIC_MESH_LOCATION + mapName);
                                    _modelNames.push_back(_strToUpper(lodFile));
                                }
                            }
                        }
                    }
                }
            }
        }
        closedir(dir);
    }
    else {
        std::cout << "Problem reading from directory!" << std::endl;
    }
    if ((dir = opendir(ANIMATED_MESH_LOCATION.c_str())) != nullptr) {
        LOG_INFO("Files to be processed: \n");

        while ((ent = readdir(dir)) != nullptr) {
            if (*ent->d_name) {
                std::string fileName = std::string(ent->d_name);
                DIR*           subDir;
                struct dirent* subEnt;
                if ((subDir = opendir((ANIMATED_MESH_LOCATION + fileName).c_str())) != nullptr) {
                    while ((subEnt = readdir(subDir)) != nullptr) {

                        if (*subEnt->d_name) {
                            LOG_INFO(subEnt->d_name, "\n");

                            std::string lodFile               = std::string(subEnt->d_name);
                            if (lodFile.find("lod") != std::string::npos) {
                                std::string mapName           = fileName + "/" + lodFile;
                                lodFile                       = lodFile.substr(0, lodFile.size() - 4);
                                _models[_strToUpper(lodFile)] = static_cast<Model*>(new AnimatedModel(ANIMATED_MESH_LOCATION + mapName));
                                _modelNames.push_back(_strToUpper(lodFile));
                            }
                        }
                    }
                }
            }
        }
        closedir(dir);
    }
    else {
        std::cout << "Problem reading from directory!" << std::endl;
    }
    if ((dir = opendir(SCENE_MESH_LOCATION.c_str())) != nullptr) {
        LOG_INFO("Files to be processed: \n");

        while ((ent = readdir(dir)) != nullptr) {
            if (*ent->d_name) {
                std::string fileName = std::string(ent->d_name);

                if (fileName.empty()      == false &&
                    fileName              != "."   &&
                    fileName              != ".."  &&
                    fileName.find(".ini") == std::string::npos) {

                    LOG_INFO(ent->d_name, "\n");

                    std::string mapName            = fileName + "/" + fileName + ".fbx";
                    _models[_strToUpper(fileName)] = new Model(SCENE_MESH_LOCATION + mapName);
                }
            }
        }
        closedir(dir);
    }
    else {
        std::cout << "Problem reading from directory!" << std::endl;
    }
}