#include "SceneManager.h"
#include "MasterClock.h"
#include "SimpleContextEvents.h"

SceneManager::SceneManager(int* argc, char** argv, unsigned int viewportWidth, unsigned int viewportHeight, float nearPlaneDistance, float farPlaneDistance) {

    _viewManager = new ViewManager(argc, argv, viewportWidth, viewportHeight);

    _modelFactory = ModelFactory::instance(); //grab static instance

    _modelFactory->setViewWrapper(_viewManager); //Set the reference to the view model event interface

    _deferredRenderer = new DeferredRenderer();
    _deferredRenderer->build("");

    _shadowRenderer = new ShadowRenderer();

    //Setup pre and post draw callback events received when a draw call is issued
    SimpleContextEvents::setPreDrawCallback(std::bind(&SceneManager::_preDraw, this));
    SimpleContextEvents::setPostDrawCallback(std::bind(&SceneManager::_postDraw, this));

    _modelList.push_back(_modelFactory->makeModel("../models/meshes/landscape/landscape.fbx")); //Add a static model to the scene

    int x = -900;
    for (int i = 0; i < 1; ++i) {
        //_modelList.push_back(_modelFactory->makeAnimatedModel("../models/meshes/troll/troll_idle.fbx")); //Add an animated model to the scene
        //_modelList.push_back(_modelFactory->makeAnimatedModel("../models/meshes/hagraven/hagraven_idle.fbx")); //Add an animated model to the scene
        //_modelList.push_back(_modelFactory->makeAnimatedModel("../models/meshes/wolf/wolf_turnleft.fbx")); //Add an animated model to the scene
        _modelList.push_back(_modelFactory->makeAnimatedModel("../models/meshes/werewolf/werewolf_jump.fbx")); //Add an animated model to the scene

        //Simple kludge test to activate animated models in motion to stimulate collision detection tests
        _modelList.back()->getStateVector()->setActive(true);
        _modelList.back()->setPosition(Vector4(0, 5, 0, 1)); //Place objects 20 meters above sea level for collision testing
        x += 30;
    }
    _viewManager->setProjection(viewportWidth, viewportHeight, nearPlaneDistance, farPlaneDistance); //Initializes projection matrix and broadcasts upate to all listeners
    _viewManager->setView(Matrix::cameraTranslation(0.0, 2.0, -20.0), Matrix(), Matrix()); //Place view 25 meters in +z direction
    _viewManager->setModelList(_modelList);

    _physics.addModels(_modelList); //Gives physics a pointer to all models which allows access to underlying geometry
    _physics.run(); //Dispatch physics to start kinematics 

    //Add a directional light pointing down in the negative y axis
    MVP lightMVP;
    lightMVP.setView(Matrix::cameraTranslation(0.0, 0.0, 25.0) * Matrix::cameraRotationAroundX(-90.0f));
    lightMVP.setProjection(Matrix::cameraOrtho(50.0, 50.0, 0.1, 100.0));
    _lightList.push_back(new Light(lightMVP, LightType::DIRECTIONAL));

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

void SceneManager::_preDraw() {

    //send all vbo data to shadow shader pre pass
    _shadowRenderer->generateShadowBuffer(_modelList, _lightList);

    //Establish an offscreen Frame Buffer Object to generate G buffers for deferred shading
    _deferredRenderer->bind();
}
void SceneManager::_postDraw() {

    //unbind fbo
    _deferredRenderer->unbind();
    //Pass lights to deferred shading pass
    _deferredRenderer->deferredLighting(_shadowRenderer, _lightList, _viewManager);
}