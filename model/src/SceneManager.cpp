#include "SceneManager.h"
#include "MasterClock.h"
#include "SimpleContextEvents.h"
#include "ViewManager.h"
#include "Factory.h"
#include "DeferredRenderer.h"
#include "ShadowRenderer.h"
#include "AudioManager.h"
#include "ForwardRenderer.h"
#include "SSAO.h"
#include "EnvironmentMap.h"
#include "ProcIsland.h"

#include <Triangle.h>

SceneManager::SceneManager(int* argc, char** argv, unsigned int viewportWidth, unsigned int viewportHeight, float nearPlaneDistance, float farPlaneDistance) {

    _viewManager = new ViewManager(argc, argv, viewportWidth, viewportHeight);

    Factory::setViewWrapper(_viewManager); //Set the reference to the view model event interface

    _deferredRenderer = new DeferredRenderer();
    _forwardRenderer = new ForwardRenderer();

    _shadowRenderer = new ShadowRenderer(8*1024, 8*1024);

    _pointShadowMap = new PointShadowMap(2000, 2000);

    _ssaoPass = new SSAO();

    //_environmentMap = new EnvironmentMap(2000, 2000);

    _audioManager = new AudioManager();

    //Setup pre and post draw callback events received when a draw call is issued
    SimpleContextEvents::setPreDrawCallback(std::bind(&SceneManager::_preDraw, this));
    SimpleContextEvents::setPostDrawCallback(std::bind(&SceneManager::_postDraw, this));

    GenerateProceduralIsland(_modelList, ProcState());

    //_physics.addModels(_modelList); //Gives physics a pointer to all models which allows access to underlying geometry
    //_physics.run(); //Dispatch physics to start kinematics

    //Add a directional light pointing down in the negative y axis
    MVP lightMVP;
    lightMVP.setView(Matrix::cameraTranslation(-50.0f, -50.0f, 0) * Matrix::cameraRotationAroundX(-90.0f));
    lightMVP.setProjection(Matrix::cameraOrtho(100, 100, 1.0f, 200));
    _lightList.push_back(Factory::make<Light>(lightMVP, LightType::CAMERA_DIRECTIONAL));

    MVP lightMapMVP;
    lightMapMVP.setView(Matrix::cameraTranslation(-50.0f, -50.0f, 0) * Matrix::cameraRotationAroundX(-90.0f));
    lightMapMVP.setProjection(Matrix::cameraOrtho(100, 100, 1.0f, 200));
    _lightList.push_back(Factory::make<Light>(lightMapMVP, LightType::MAP_DIRECTIONAL));


    //Model view projection matrix for point light additions
    MVP pointLightMVP;

    //point light projection has a 90 degree view angle with a 1 aspect ratio for generating square shadow maps
    //with a near z value of 1 and far z value of 100
    pointLightMVP.setProjection(Matrix::cameraProjection(90.0f, 1.0f, 1.0f, 100.0f));

    //Placing the lights in equidistant locations for testing
    pointLightMVP.setModel(Matrix::translation(0.0f, 5.0, 0.0f));
    _lightList.push_back(Factory::make<Light>(pointLightMVP, LightType::POINT, Vector4(1.0f, 0.4f, 0.1f, 1.0f), true));
    //pointLightMVP.setModel(Matrix::translation(190.0f, 20.0, 185.0f));
    //_lightList.push_back(Factory::make<Light>(pointLightMVP, LightType::POINT, Vector4(1.0f, 0.4f, 0.1f, 1.0f), false));
    //pointLightMVP.setModel(Matrix::translation(0.0f, 40, -50.0f));
    //_lightList.push_back(Factory::make<Light>(pointLightMVP, LightType::POINT, Vector4(0.0f, 0.0f, 1.0f, 1.0f)));
    /*pointLightMVP.setModel(Matrix::translation(0.0f, 25.0f, -100.0f));
    _lightList.push_back(Factory::make<Light>(pointLightMVP, LightType::POINT, Vector4(1.0f, 1.0f, 1.0f, 1.0f)));
    pointLightMVP.setModel(Matrix::translation(-100.0f, 25.0f, 0.0f));
    _lightList.push_back(Factory::make<Light>(pointLightMVP, LightType::POINT, Vector4(1.0f, 0.0f, 1.0f, 1.0f)));*/

    MasterClock::instance()->run(); //Scene manager kicks off the clock event manager

    _audioManager->StartAll();

    // Do this after adding all of our objects.
    _viewManager->setProjection(viewportWidth, viewportHeight, nearPlaneDistance, farPlaneDistance); //Initializes projection matrix and broadcasts upate to all listeners
                                                                                                     // This view is carefully chosen to look at a mountain without showing the (lack of) water in the scene.
    _viewManager->setView(Matrix::cameraTranslation(0.f, 0.68f, 20.f),
        Matrix::cameraRotationAroundY(-45.f),
        Matrix());
    _viewManager->setModelList(_modelList);

    _viewManager->run(); //Enables the glfw main loop
}

SceneManager::~SceneManager() {
    for (auto model : _modelList) {
        delete model;
    }
    delete _shadowRenderer;
    delete _deferredRenderer;
    delete _viewManager;
    delete _audioManager;
    delete _forwardRenderer;
}

void SceneManager::_preDraw() {

    //send all vbo data to shadow shader pre pass
    _shadowRenderer->generateShadowBuffer(_modelList, _lightList);

    //send all vbo data to point light shadow pre pass
    for (Light* light : _lightList) {
        _pointShadowMap->render(_modelList, light);
    }

    //Disable environment mapping onto a texture cube atm
    //MVP mvp;
    //mvp.setView(_viewManager->getView());
    //mvp.setProjection(_viewManager->getProjection());
    //_environmentMap->render(_modelList, &mvp);

    //Establish an offscreen Frame Buffer Object to generate G buffers for deferred shading
    _deferredRenderer->bind();
}
void SceneManager::_postDraw() {

    //unbind fbo
    _deferredRenderer->unbind();

    //Only compute ssao for opaque objects
    _ssaoPass->computeSSAO(_deferredRenderer->getGBuffers(), _viewManager);

    //Render on default color, depth and stencil buffers
    //Clear color buffer from frame buffer otherwise framebuffer will contain data from the last draw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Pass lights to deferred shading pass
    _deferredRenderer->deferredLighting(_shadowRenderer, _lightList, _viewManager, _pointShadowMap, _ssaoPass, _environmentMap);

    //Draw transparent objects onto of the deferred renderer
    _forwardRenderer->forwardLighting(_modelList, _viewManager, _shadowRenderer, _lightList, _pointShadowMap);

    for (auto light : _lightList) {
        if (light->getType() == LightType::POINT) {
            light->renderLight();
        }
    }
}
