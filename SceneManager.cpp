#include "SceneManager.h"
#include "MasterClock.h"

SceneManager::SceneManager(int* argc, char** argv, unsigned int viewportWidth, unsigned int viewportHeight, float nearPlaneDistance, float farPlaneDistance) {

    _viewManager = new ViewManager(argc, argv, viewportWidth, viewportHeight);

    _modelFactory = ModelFactory::instance(); //grab static instance

    _modelFactory->setViewWrapper(_viewManager); //Set the reference to the view model event interface

    _modelList.push_back(_modelFactory->makeModel("../models/meshes/landscape/landscape.fbx")); //Add a static model to the scene
    
    for(int i = 0; i < 1; ++i) {
        //_modelList.push_back(_modelFactory->makeAnimatedModel("../models/meshes/troll/troll_idle.fbx")); //Add an animated model to the scene
        _modelList.push_back(_modelFactory->makeAnimatedModel("../models/meshes/hagraven/hagraven_idle.fbx")); //Add an animated model to the scene
        //_modelList.push_back(_modelFactory->makeAnimatedModel("../models/meshes/human/human_idle.fbx")); //Add an animated model to the scene
        //_modelList.push_back(_modelFactory->makeAnimatedModel("../models/meshes/wolf/wolf_turnleft.fbx")); //Add an animated model to the scene
        //_modelList.push_back(_modelFactory->makeAnimatedModel("../models/meshes/werewolf/werewolf_jump.fbx")); //Add an animated model to the scene
    }
    _viewManager->setProjection(viewportWidth, viewportHeight, nearPlaneDistance, farPlaneDistance); //Initializes projection matrix and broadcasts upate to all listeners
    _viewManager->setView(Matrix::cameraTranslation(0.0, 2.0, 10.0), Matrix(), Matrix()); //Place view 25 meters in +z direction

    MasterClock::instance()->run(); //Scene manager kicks off the clock event manager

    _viewManager->run(); //Enables the glut main loop
}

SceneManager::~SceneManager() {
    for (auto model : _modelList) {
        delete model;
    }
    delete _modelFactory;
    delete _viewManager;
}