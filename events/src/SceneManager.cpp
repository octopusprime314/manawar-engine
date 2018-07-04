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
#include "DeferredFrameBuffer.h"
#include "MergeShader.h"
#include "Bloom.h"
#include <Triangle.h>
#include "SSCompute.h"
#include "ShaderBroker.h"
#include "Terminal.h"
#include <chrono>

using namespace std::chrono;

uint64_t nowMs();

// We define this here because this file is basically main.
volatile bool g_AssertOnBadOpenGlCall = false;

ShaderBroker*  SceneManager::_shaderManager = ShaderBroker::instance();

SceneManager::SceneManager(int* argc, char** argv,
    unsigned int viewportWidth, unsigned int viewportHeight,
    float nearPlaneDistance, float farPlaneDistance) {
    _viewManager = new ViewManager(argc, argv, viewportWidth, viewportHeight);
    glCheck();

    Factory::setViewWrapper(_viewManager); //Set the reference to the view model event interface

    //Load and compile all shaders for the shader broker
    _shaderManager->compileShaders();
    glCheck();

    _deferredRenderer = new DeferredRenderer();
    glCheck();

    _forwardRenderer = new ForwardRenderer();
    glCheck();

    _shadowRenderer = new ShadowRenderer(8 * 1024, 8 * 1024);
    glCheck();

    _pointShadowMap = new PointShadowMap(2000, 2000);
    glCheck();

    _ssaoPass = new SSAO();
    glCheck();

    //_environmentMap = new EnvironmentMap(2000, 2000);
    //glCheck();

    _water = new Water(_viewManager->getEventWrapper());
    glCheck();

    _audioManager = new AudioManager();
    glCheck();

    _deferredFBO = new DeferredFrameBuffer();
    glCheck();

    _mergeShader = static_cast<MergeShader*>(_shaderManager->getShader("mergeShader"));
    glCheck();

    _bloom = new Bloom();
    glCheck();

    _deferredFBO = new DeferredFrameBuffer();
    glCheck();

    _terminal = new Terminal();

    _add = new SSCompute("add", screenPixelWidth, screenPixelHeight, TextureFormat::RGBA_UNSIGNED_BYTE);

    //Setup pre and post draw callback events received when a draw call is issued
    SimpleContextEvents::setPreDrawCallback(std::bind(&SceneManager::_preDraw, this));
    SimpleContextEvents::setPostDrawCallback(std::bind(&SceneManager::_postDraw, this));

    //GenerateProceduralIsland(_modelList, ProcState());
    //glCheck();

    _modelList.push_back(Factory::make<Model>("landscape/landscape.fbx")); //Add a static model to the scene
    _modelList.push_back(Factory::make<AnimatedModel>("werewolf/werewolf_jump.fbx")); //Add a static model to the scene

    //_physics.addModels(_modelList); //Gives physics a pointer to all models which allows access to underlying geometry
    //_physics.run(); //Dispatch physics to start kinematics

    //Add a directional light pointing down in the negative y axis

    MVP lightMVP;
    lightMVP.setView(Matrix::cameraTranslation(0.0f, 0.0f, 300.0f) * Matrix::cameraRotationAroundX(-90.0f));
    lightMVP.setProjection(Matrix::cameraOrtho(200.0f, 200.0f, 1.0f, 600.0f));
    _lightList.push_back(Factory::make<Light>(lightMVP, LightType::CAMERA_DIRECTIONAL));

    MVP lightMapMVP;
    lightMapMVP.setView(Matrix::cameraTranslation(0.0f, 0.0f, 300.0f) * Matrix::cameraRotationAroundX(-90.0f));
    lightMapMVP.setProjection(Matrix::cameraOrtho(600.0f, 600.0f, 1.0f, 600.0f));
    _lightList.push_back(Factory::make<Light>(lightMapMVP, LightType::MAP_DIRECTIONAL));

    //Model view projection matrix for point light additions
    MVP pointLightMVP;

    //point light projection has a 90 degree view angle with a 1 aspect ratio for generating square shadow maps
    //with a near z value of 1 and far z value of 100
    pointLightMVP.setProjection(Matrix::cameraProjection(90.0f, 1.0f, 1.0f, 100.0f));

    //Placing the lights in equidistant locations for testing
    pointLightMVP.setModel(Matrix::translation(212.14f, 24.68f, 186.46f));
    _lightList.push_back(Factory::make<Light>(pointLightMVP,
        LightType::POINT,
        EffectType::Fire,
        Vector4(1.0f, 0.8f, 0.3f, 1.0f),
        true));

    pointLightMVP.setModel(Matrix::translation(198.45f, 24.68f, 186.71f));
    _lightList.push_back(Factory::make<Light>(pointLightMVP,
        LightType::POINT,
        EffectType::Fire,
        Vector4(1.0f, 0.8f, 0.3f, 1.0f),
        false));

    pointLightMVP.setModel(Matrix::translation(178.45f, 143.59f, 240.71f));
    _lightList.push_back(Factory::make<Light>(pointLightMVP,
        LightType::POINT,
        EffectType::Smoke,
        Vector4(0.4f, 0.4f, 0.4f, 1.0f),
        false));

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

    if (_viewManager->getViewState() == ViewManager::ViewState::DEFERRED_LIGHTING) {


        //Bind frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, _deferredFBO->getFrameBufferContext());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Pass lights to deferred shading pass
        _deferredRenderer->deferredLighting(_shadowRenderer, _lightList, _viewManager, _pointShadowMap, _ssaoPass, _environmentMap);

        //Draw transparent objects onto of the deferred renderer
        _forwardRenderer->forwardLighting(_modelList, _viewManager, _shadowRenderer, _lightList, _pointShadowMap);

        // Lights - including the fire point lights
        for (auto light : _lightList) {
            if (light->getType() == LightType::POINT) {
                light->render();
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //Compute bloom from deferred fbo texture
        _bloom->compute(_deferredFBO->getTexture());

        //If adding a second texture then all writes are to this texture second param
        _add->compute(_deferredFBO->getTexture(), _bloom->getTexture());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //_mergeShader->runShader(_deferredFBO->getTextureContext(), _bloom->getTextureContext());
        GLuint velocityBufferContext = _deferredRenderer->getGBuffers()->getTextureContexts()[3];
        _mergeShader->runShader(_bloom->getTextureContext(), velocityBufferContext);
    }
    else if (_viewManager->getViewState() == ViewManager::ViewState::DEFERRED_LIGHTING_NO_BLOOM) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Pass lights to deferred shading pass
        _deferredRenderer->deferredLighting(_shadowRenderer, _lightList, _viewManager, _pointShadowMap, _ssaoPass, _environmentMap);

        //Draw transparent objects onto of the deferred renderer
        _forwardRenderer->forwardLighting(_modelList, _viewManager, _shadowRenderer, _lightList, _pointShadowMap);

        // Lights - including the fire point lights
        for (auto light : _lightList) {
            if (light->getType() == LightType::POINT) {
                light->render();
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    else {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //Pass lights to deferred shading pass
        _deferredRenderer->deferredLighting(_shadowRenderer, _lightList, _viewManager, _pointShadowMap, _ssaoPass, _environmentMap);
    }

    _terminal->display();
}