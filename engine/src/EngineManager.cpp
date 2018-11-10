#include "EngineManager.h"
#include "MasterClock.h"
#include "IOEvents.h"
#include "ViewEventDistributor.h"
#include "Factory.h"
#include "DeferredRenderer.h"
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
#include "FrustumCuller.h"
#include "Entity.h"
#include "ModelBroker.h"
#include "ShadowedDirectionalLight.h"
#include "ShadowedPointLight.h"
#include "IOEventDistributor.h"
#include "Picker.h"
#include "DXLayer.h"
#include <chrono>

using namespace std::chrono;

uint64_t nowMs();

// We define this here because this file is basically main.
volatile bool g_AssertOnBadOpenGlCall = false;

std::vector<Entity*> EngineManager::_entityList;
GraphicsLayer EngineManager::_graphicsLayer;

EngineManager::EngineManager(int* argc, char** argv, HINSTANCE hInstance, int nCmdShow) {

    _graphicsLayer = GraphicsLayer::DX12;

    if (_graphicsLayer == GraphicsLayer::OPENGL) {
        //Create instance of glfw wrapper class context
        //GLFW context can only run on main thread!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //DO NOT THREAD GLFW CALLS
        _glfwContext = new IOEventDistributor(argc, argv);
    }
    else {
        DXLayer::initialize(hInstance, IOEventDistributor::screenPixelWidth, IOEventDistributor::screenPixelHeight, nCmdShow);
        _dxLayer = DXLayer::instance();
    }


    //Load and compile all shaders for the shader broker
    ShaderBroker::instance()->compileShaders();

    _deferredRenderer = new DeferredRenderer();

    _viewManager = new ViewEventDistributor(argc, argv, IOEventDistributor::screenPixelWidth, IOEventDistributor::screenPixelHeight);
    
    ModelBroker::setViewManager(_viewManager); //Set the reference to the view model event interface
    _viewManager->setProjection(IOEventDistributor::screenPixelWidth, IOEventDistributor::screenPixelHeight, 0.1f, 5000.0f); //Initializes projection matrix and broadcasts upate to all listeners
     // This view is carefully chosen to look at a mountain without showing the (lack of) water in the scene.
    _viewManager->setView(Matrix::cameraTranslation(0.0f, 20.0f, -200.0f),
        Matrix::cameraRotationAroundY(0.0f),
        Matrix());

    //Load and compile all models for the model broker
    ModelBroker::instance()->buildModels();

    //Setup pre and post draw callback events received when a draw call is issued
    IOEvents::setPreDrawCallback(std::bind(&EngineManager::_preDraw, this));
    IOEvents::setPostDrawCallback(std::bind(&EngineManager::_postDraw, this));

    auto modelBroker = ModelBroker::instance();

    //_entityList.push_back(new Entity(modelBroker->getModel("werewolf"), _viewManager->getEventWrapper())); //Add a static model to the scene

    _physics = new Physics();
    _physics->addEntities(_entityList); //Gives physics a pointer to all models which allows access to underlying geometry

    _physics->run(); //Dispatch physics to start kinematics

    if (_graphicsLayer == GraphicsLayer::DX12) {

        _forwardRenderer = new ForwardRenderer();
        _ssaoPass = new SSAO();

        _dxLayer->flushCommandList();
        
        _viewManager->triggerEvents();
        _viewManager->setEntityList(_entityList);


        MasterClock::instance()->run(); //Scene manager kicks off the clock event manager
        
        Vector4 sunLocation(0.0f, 0.0f, -300.0f);
        MVP lightMapMVP;
        
        lightMapMVP.setView(Matrix::translation(sunLocation.getx(), sunLocation.gety(), sunLocation.getz())
            * Matrix::cameraRotationAroundX(-90.0f));
        
        lightMapMVP.setProjection(Matrix::cameraOrtho(200.0f, 200.0f, 0.0f, 600.0f));
        
        std::vector<Light*> lightList;
        lightList.push_back(new ShadowedDirectionalLight(_viewManager->getEventWrapper(),
            lightMapMVP,
            EffectType::None,
            Vector4(1.0, 0.0, 0.0)));

        _dxLayer->run(_deferredRenderer, 
            _entityList, 
            lightList, 
            _viewManager,
            _forwardRenderer,_ssaoPass);

        //Eventually replace _dxLayer->run with _glfwContext->run
        //_glfwContext->run();

    }
    else {

        _forwardRenderer = new ForwardRenderer();

        _ssaoPass = new SSAO();

        //_environmentMap = new EnvironmentMap(2000, 2000);

        _water = new Water(_viewManager->getEventWrapper());

        _audioManager = new AudioManager();

        _deferredFBO = new DeferredFrameBuffer();

        _mergeShader = static_cast<MergeShader*>(ShaderBroker::instance()->getShader("mergeShader"));

        _bloom = new Bloom();

        _add = new SSCompute("add", IOEventDistributor::screenPixelWidth, IOEventDistributor::screenPixelHeight, TextureFormat::RGBA_UNSIGNED_BYTE);

        _terminal = new Terminal(_deferredRenderer->getGBuffers(), _entityList);

        Vector4 sunLocation(0.0f, 0.0f, -300.0f);
        MVP lightMVP;
        lightMVP.setView(Matrix::translation(sunLocation.getx(), sunLocation.gety(), sunLocation.getz())
            * Matrix::cameraRotationAroundX(-90.0f));
        lightMVP.setProjection(Matrix::cameraOrtho(200.0f, 200.0f, 0.0f, 600.0f));
        _lightList.push_back(new ShadowedDirectionalLight(_viewManager->getEventWrapper(),
            lightMVP,
            EffectType::None,
            Vector4(1.0, 0.0, 0.0)));

        MVP lightMapMVP;
        lightMapMVP.setView(Matrix::translation(sunLocation.getx(), sunLocation.gety(), sunLocation.getz())
            * Matrix::cameraRotationAroundX(-90.0f));
        lightMapMVP.setProjection(Matrix::cameraOrtho(600.0f, 600.0f, 0.0f, 600.0f));
        _lightList.push_back(new ShadowedDirectionalLight(_viewManager->getEventWrapper(),
            lightMapMVP,
            EffectType::None,
            Vector4(1.0, 0.0, 0.0)));

        //Model view projection matrix for point light additions
        MVP pointLightMVP;

        //point light projection has a 90 degree view angle with a 1 aspect ratio for generating square shadow maps
        //with a near z value of 1 and far z value of 100
        pointLightMVP.setProjection(Matrix::cameraProjection(90.0f, 1.0f, 1.0f, 100.0f));

        //Placing the lights in equidistant locations for testing
        pointLightMVP.setModel(Matrix::translation(212.14f, 24.68f, 186.46f));
        _lightList.push_back(new ShadowedPointLight(_viewManager->getEventWrapper(),
            pointLightMVP,
            EffectType::Fire,
            Vector4(1.0f, 0.8f, 0.3f, 1.0f)));

        pointLightMVP.setModel(Matrix::translation(198.45f, 24.68f, 186.71f));
        _lightList.push_back(new Light(_viewManager->getEventWrapper(),
            pointLightMVP,
            LightType::POINT,
            EffectType::Fire,
            Vector4(1.0f, 0.8f, 0.3f, 1.0f)));

        pointLightMVP.setModel(Matrix::translation(178.45f, 143.59f, 240.71f));
        _lightList.push_back(new Light(_viewManager->getEventWrapper(),
            pointLightMVP,
            LightType::POINT,
            EffectType::Smoke,
            Vector4(0.4f, 0.4f, 0.4f, 1.0f)));

        pointLightMVP.setModel(Matrix::translation(0.0f, 10.0f, 0.0f));
        _lightList.push_back(new Light(_viewManager->getEventWrapper(),
            pointLightMVP,
            LightType::POINT,
            EffectType::Fire,
            Vector4(1.0f, 0.8f, 0.3f, 1.0f)));

        MasterClock::instance()->run(); //Scene manager kicks off the clock event manager

        //_audioManager->StartAll();

        _viewManager->triggerEvents();
        _viewManager->setEntityList(_entityList);

        _glfwContext->run();
    }
}

EngineManager::~EngineManager() {
    for (auto entity : _entityList) {
        delete entity;
    }
    delete _deferredRenderer;
    delete _viewManager;
    delete _audioManager;
    delete _forwardRenderer;
}

Entity* EngineManager::addEntity(Model* model, Matrix transform) {
    auto viewManager = ModelBroker::getViewManager();
    auto viewWrapper = viewManager->getEventWrapper();
    MVP mvp;
    mvp.setModel(transform);
    mvp.setView(viewManager->getView());
    mvp.setProjection(viewManager->getProjection());
    _entityList.push_back(new Entity(model, viewWrapper, mvp)); //Add a static model to the scene
    return _entityList.back();
}

GraphicsLayer EngineManager::getGraphicsLayer() {
    return _graphicsLayer;
}

std::vector<Entity*>* EngineManager::getEntityList() {
    return &_entityList;
}

void EngineManager::_preDraw() {
    glCheck();

    if (_viewManager->getViewState() == Camera::ViewState::POINT_SHADOW ||
        _viewManager->getViewState() == Camera::ViewState::DEFERRED_LIGHTING ||
        _viewManager->getViewState() == Camera::ViewState::CAMERA_SHADOW ||
        _viewManager->getViewState() == Camera::ViewState::MAP_SHADOW) {
       
        //send all vbo data to point light shadow pre pass
        for (Light* light : _lightList) {
            light->renderShadow(_entityList);
        }
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
void EngineManager::_postDraw() {
    glCheck();
    //Render the water around the island
    _water->render();

    //unbind fbo
    _deferredRenderer->unbind();

    if (_viewManager->getViewState() == Camera::ViewState::DEFERRED_LIGHTING) {

        //Only compute ssao for opaque objects
        _ssaoPass->computeSSAO(_deferredRenderer->getGBuffers(), _viewManager);

        //Bind frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, _deferredFBO->getFrameBufferContext());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Pass lights to deferred shading pass
        _deferredRenderer->deferredLighting(_lightList, _viewManager, _ssaoPass, _environmentMap);

        //Draw transparent objects onto of the deferred renderer
        _forwardRenderer->forwardLighting(_entityList, _viewManager, _lightList);

        // Lights - including the fire point lights
        for (auto light : _lightList) {
            light->render();
        }

        for (auto entity : _entityList) {
            if (entity->getSelected()) {
                entity->getFrustumCuller()->visualize();
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //Compute bloom from deferred fbo texture
        _bloom->compute(_deferredFBO->getRenderTexture());

        //If adding a second texture then all writes are to this texture second param
        _add->compute(_deferredFBO->getRenderTexture(), _bloom->getTexture());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Texture* velocityTexture = &_deferredRenderer->getGBuffers()->getTextures()[2];
        _mergeShader->runShader(_bloom->getTexture(), velocityTexture);
    }
    else if (_viewManager->getViewState() == Camera::ViewState::PHYSICS) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _physics->visualize();
        //_entityList[1]->getFrustumCuller()->visualize();
        //_viewManager->displayViewFrustum();

        ////shows all of the light/shadow volumes
        //for (Light* light : _lightList) {
        //    light->renderDebug();
        //}
    }
    else {

        //Only compute ssao for opaque objects
        _ssaoPass->computeSSAO(_deferredRenderer->getGBuffers(), _viewManager);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //Pass lights to deferred shading pass
        _deferredRenderer->deferredLighting(_lightList, _viewManager, _ssaoPass, _environmentMap);
    }

    _terminal->display();
}