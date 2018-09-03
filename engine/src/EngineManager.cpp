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
#include <chrono>

using namespace std::chrono;

uint64_t nowMs();

// We define this here because this file is basically main.
volatile bool g_AssertOnBadOpenGlCall = false;

std::vector<Entity*> EngineManager::_entityList;

EngineManager::EngineManager(int* argc, char** argv) {

    //Create instance of glfw wrapper class context
    //GLFW context can only run on main thread!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //DO NOT THREAD GLFW CALLS
    _glfwContext = new IOEventDistributor(argc, argv);

    //Load and compile all shaders for the shader broker
    ShaderBroker::instance()->compileShaders();

    _viewManager = new ViewEventDistributor(argc, argv, IOEventDistributor::screenPixelWidth, IOEventDistributor::screenPixelHeight);
    
    ModelBroker::setViewManager(_viewManager); //Set the reference to the view model event interface
    _viewManager->setProjection(IOEventDistributor::screenPixelWidth, IOEventDistributor::screenPixelHeight, 0.1f, 5000.0f); //Initializes projection matrix and broadcasts upate to all listeners
     // This view is carefully chosen to look at a mountain without showing the (lack of) water in the scene.
    _viewManager->setView(Matrix::cameraTranslation(0.0f, 0.0f, 0.0f),
        Matrix::cameraRotationAroundY(0.0f),
        Matrix());

    glCheck();

    //Load and compile all shaders for the shader broker
    ModelBroker::instance()->buildModels();
    glCheck();

    _deferredRenderer = new DeferredRenderer();
    glCheck();

    _forwardRenderer = new ForwardRenderer();
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

    _mergeShader = static_cast<MergeShader*>(ShaderBroker::instance()->getShader("mergeShader"));
    glCheck();

    _bloom = new Bloom();
    glCheck();

    _deferredFBO = new DeferredFrameBuffer();
    glCheck();

    _add = new SSCompute("add", IOEventDistributor::screenPixelWidth, IOEventDistributor::screenPixelHeight, TextureFormat::RGBA_UNSIGNED_BYTE);

    //Setup pre and post draw callback events received when a draw call is issued
    IOEvents::setPreDrawCallback(std::bind(&EngineManager::_preDraw, this));
    IOEvents::setPostDrawCallback(std::bind(&EngineManager::_postDraw, this));

    auto modelBroker = ModelBroker::instance();

    //_entityList.push_back(new Entity(modelBroker->getModel("sandbox"), _viewManager->getEventWrapper())); //Add a static model to the scene
    _entityList.push_back(new Entity(modelBroker->getModel("werewolf"), _viewManager->getEventWrapper())); //Add a static model to the scene
    //_entityList.push_back(new Entity(modelBroker->getModel("wolf"), _viewManager->getEventWrapper())); //Add a static model to the scene
    //_entityList.push_back(new Entity(modelBroker->getModel("hagraven"), _viewManager->getEventWrapper())); //Add a static model to the scene
    //_entityList.push_back(new Entity(modelBroker->getModel("troll"), _viewManager->getEventWrapper())); //Add a static model to the scene

    //_entityList[0]->setPosition(Vector4(30, 0, 10));
    //_entityList[2]->setPosition(Vector4(-10, 0, -10));
    //_entityList[3]->setPosition(Vector4(30, 0, 10));
    //_entityList[4]->setPosition(Vector4(30, 0, -20));

    _terminal = new Terminal(_deferredRenderer->getGBuffers(), _entityList);

    _physics = new Physics();
    _physics->addEntities(_entityList); //Gives physics a pointer to all models which allows access to underlying geometry
    
    _physics->run(); //Dispatch physics to start kinematics

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

EngineManager::~EngineManager() {
    for (auto entity : _entityList) {
        delete entity;
    }
    delete _deferredRenderer;
    delete _viewManager;
    delete _audioManager;
    delete _forwardRenderer;
}

void EngineManager::addEntity(Model* model, Matrix transform) {
    auto viewManager = ModelBroker::getViewManager();
    auto viewWrapper = viewManager->getEventWrapper();
    MVP mvp;
    mvp.setModel(transform);
    mvp.setView(viewManager->getView());
    mvp.setProjection(viewManager->getProjection());
    _entityList.push_back(new Entity(model, viewWrapper, mvp)); //Add a static model to the scene
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
        _bloom->compute(_deferredFBO->getTexture());

        //If adding a second texture then all writes are to this texture second param
        _add->compute(_deferredFBO->getTexture(), _bloom->getTexture());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GLuint velocityBufferContext = _deferredRenderer->getGBuffers()->getTextureContexts()[2];
        _mergeShader->runShader(_bloom->getTextureContext(), velocityBufferContext);
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