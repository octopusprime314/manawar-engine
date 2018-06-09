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
#include "Water.h"
#include "Font.h"

#include <Triangle.h>

// We define this here because this file is basically main.
volatile bool g_AssertOnBadOpenGlCall = false;

SceneManager::SceneManager(int* argc, char** argv, unsigned int viewportWidth, unsigned int viewportHeight, float nearPlaneDistance, float farPlaneDistance) {
    _viewManager = new ViewManager(argc, argv, viewportWidth, viewportHeight);
    glCheck();

    Factory::setViewWrapper(_viewManager); //Set the reference to the view model event interface

    _deferredRenderer = new DeferredRenderer();
    glCheck();

    _forwardRenderer = new ForwardRenderer();
    glCheck();

    _shadowRenderer = new ShadowRenderer(8*1024, 8*1024);
    glCheck();

    _pointShadowMap = new PointShadowMap(2000, 2000);
    glCheck();

    _ssaoPass = new SSAO();
    glCheck();

    //_environmentMap = new EnvironmentMap(2000, 2000);
    glCheck();

    _water = new Water(_viewManager->getEventWrapper());
    glCheck();

    _audioManager = new AudioManager();
    glCheck();

    _fontRenderer = new FontRenderer("ubuntu_mono_regular.fnt");
    glCheck();

    //Setup pre and post draw callback events received when a draw call is issued
    SimpleContextEvents::setPreDrawCallback(std::bind(&SceneManager::_preDraw, this));
    SimpleContextEvents::setPostDrawCallback(std::bind(&SceneManager::_postDraw, this));

    GenerateProceduralIsland(_modelList, ProcState());
    glCheck();

    //_modelList.push_back(Factory::make<Model>("landscape/landscape.fbx")); //Add a static model to the scene

    //_physics.addModels(_modelList); //Gives physics a pointer to all models which allows access to underlying geometry
    //_physics.run(); //Dispatch physics to start kinematics

    //Add a directional light pointing down in the negative y axis
    {
        const float size = 30.f;
        const float r = 15;  // Right
        const float l = 0;   // Left
        const float t = 30;  // Top
        const float b = -30; // Bottom
        const float f = 30;  // Far
        const float n = -30; // Near
        const float ortho[16] = {
            2.f / (r - l), 0.f,           0.f,            -(r + l) / (r - l),
            0.f,           2.f / (t - b), 0.f,            -(t + b) / (t - b),
            0.f,           0.f,           -2.f / (f - n), -(f + n) / (f - n),
            0.f,           0.f,           0.f,            1.f
        };
        Matrix lightProj(Matrix::cameraOrtho(14, 35, -35, 35));
        //Matrix lightProj(Matrix::cameraOrtho(35, 35, -35, 35));

        MVP lightMVP;
        lightMVP.setProjection(lightProj);
        _lightList.push_back(Factory::make<Light>(lightMVP, LightType::CAMERA_DIRECTIONAL));

        MVP lightMapMVP;
        lightMapMVP.setProjection(lightProj);
        _lightList.push_back(Factory::make<Light>(lightMapMVP, LightType::MAP_DIRECTIONAL));
    }

    //Model view projection matrix for point light additions
    MVP pointLightMVP;

    //point light projection has a 90 degree view angle with a 1 aspect ratio for generating square shadow maps
    //with a near z value of 1 and far z value of 100
    pointLightMVP.setProjection(Matrix::cameraProjection(10.0f, 1.0f, 1.0f, 100.0f));

    //Placing the lights in equidistant locations for testing
    pointLightMVP.setModel(Matrix::translation(-6.52151155f, 0.680000007f, 2.27768421f));
    _lightList.push_back(Factory::make<Light>(pointLightMVP, LightType::POINT, EffectType::Fire, Vector4(1.0f, 0.4f, 0.1f, 1.0f), true));

    pointLightMVP.setModel(Matrix::translation(-6.32151155f, 0.780000007f, 2.57768421f));
    _lightList.push_back(Factory::make<Light>(pointLightMVP, LightType::POINT, EffectType::Fire, Vector4(1.0f, 0.4f, 0.1f, 1.0f), false));

    pointLightMVP.setModel(Matrix::translation(-6.72151155f, 0.730000007f, 2.77768421f));
    _lightList.push_back(Factory::make<Light>(pointLightMVP, LightType::POINT, EffectType::Fire, Vector4(1.0f, 0.4f, 0.1f, 1.0f), false));

    pointLightMVP.setModel(Matrix::translation(5.25, 3.38, -1.14));
    _lightList.push_back(Factory::make<Light>(pointLightMVP, LightType::POINT, EffectType::Smoke, Vector4(0.2f, 0.2f, 0.2f, 1.0f), false));
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
    _viewManager->setView(Matrix::cameraTranslation(7.f, -0.68f, -5.f),
        Matrix::cameraRotationAroundY(-45.f),
        Matrix());
    _viewManager->setModelList(_modelList);

    glCheck();

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
    glCheck();

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
    glCheck();
}
void SceneManager::_postDraw() {
    glCheck();

    //Render the water around the island
    _water->render();

    //unbind fbo
    _deferredRenderer->unbind();

    //Only compute ssao for opaque objects
    _ssaoPass->computeSSAO(_deferredRenderer->getGBuffers(), _viewManager);

    //Render on default color, depth and stencil buffers
    //Clear color buffer from frame buffer otherwise framebuffer will contain data from the last draw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Pass lights to deferred shading pass
    _deferredRenderer->deferredLighting(_shadowRenderer, _lightList, _viewManager, _pointShadowMap, _ssaoPass, _environmentMap);

    if (_viewManager->getViewState() == ViewManager::ViewState::DEFERRED_LIGHTING) {

        //Draw transparent objects onto of the deferred renderer
        _forwardRenderer->forwardLighting(_modelList, _viewManager, _shadowRenderer, _lightList, _pointShadowMap);
        
        // Lights - including the fire point lights
        for (auto light : _lightList) {
            if (light->getType() == LightType::POINT) {
                light->render();
            }
        }
    }

    std::string stringToDraw("hello hawaii");
    _fontRenderer->DrawFont(0, 0, stringToDraw);
    glCheck();
}
