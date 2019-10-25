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
#include "HLSLShader.h"
#include "BlitDepthShader.h"
#include "WorldGenerator.h"
#include <chrono>

using namespace std::chrono;
uint64_t nowMs();
// We define this here because this file is basically main.
volatile bool g_AssertOnBadOpenGlCall = false;

RayTracingPipelineShader* EngineManager::_rayTracingPipeline = nullptr;
Entity*                   EngineManager::_shadowEntity       = nullptr;
std::mutex                EngineManager::_entityListLock;
GraphicsLayer             EngineManager::_graphicsLayer;
std::vector<Entity*>      EngineManager::_entityList;

EngineManager::EngineManager(int*      argc,
                             char**    argv,
                             HINSTANCE hInstance,
                             int       nCmdShow) {

    _graphicsLayer = GraphicsLayer::OPENGL;
    _generatorMode = true;

    if (_graphicsLayer >= GraphicsLayer::DX12) {

        // Disables the need for the dxil.dll validation step in the compiler chain
        D3D12EnableExperimentalFeatures(1,
                                        &D3D12ExperimentalShaderModels,
                                        nullptr,
                                        0);

        DXLayer::initialize(hInstance, nCmdShow);

        if (_graphicsLayer == GraphicsLayer::DXR_EXPERIMENTAL) {
            auto device                                     = DXLayer::instance()->getDevice();
            D3D12_FEATURE_DATA_SHADER_MODEL shaderModel     = { D3D_SHADER_MODEL_6_5 };
            HRESULT                         supportForSM6_5 = device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL,
                                                                                          &shaderModel,
                                                                                          sizeof(D3D12_FEATURE_DATA_SHADER_MODEL));

            if (D3D_SHADER_MODEL_6_5 == shaderModel.HighestShaderModel) {
                //do something lol
            } 
            else {
                assert("Why isn't there support for this feature...probably the D3D12.dll doesn't support it");
            }
        }
    }

    // disable raytracing until i find a way to not tank performance for shadows
    _useRaytracing = ((_graphicsLayer == GraphicsLayer::DXR)                              ||
                      (_graphicsLayer == GraphicsLayer::DXR_EXPERIMENTAL) ? true : false) &&
                      DXLayer::instance()->supportsRayTracing();

    _inputLayer = new IOEventDistributor(   argc,
                                            argv,
                                            hInstance,
                                            nCmdShow);

    _viewManager = new ViewEventDistributor(argc,
                                            argv,
                                            IOEventDistributor::screenPixelWidth,
                                            IOEventDistributor::screenPixelHeight);
    
    //Set the reference to the view model event interface
    ModelBroker::setViewManager(_viewManager);

    //Initializes projection matrix and broadcasts upate to all listeners
    _viewManager->setProjection(IOEventDistributor::screenPixelWidth,
                                IOEventDistributor::screenPixelHeight,
                                0.1f,
                                10000.0f);

    _viewManager->setView(      Matrix::translation(0, 2000.0f, -3500.0f),
                                Matrix::rotationAroundX(-70.0f),
                                Matrix());

    //Load and compile all shaders for the shader broker
    ShaderBroker::instance()->compileShaders();
    //Load and compile all models for the model broker
    ModelBroker::instance()->buildModels();

    //Setup pre and post draw callback events received when a draw call is issued
    IOEvents::setPreDrawCallback( std::bind(&EngineManager::_preDraw, this));
    IOEvents::setPostDrawCallback(std::bind(&EngineManager::_postDraw, this));

    auto modelBroker = ModelBroker::instance();

    //Add a static model to the scene
    //_entityList.push_back(new Entity(modelBroker->getModel("wolf"), _viewManager->getEventWrapper()));

    _deferredRenderer = new DeferredRenderer();
    _forwardRenderer  = new ForwardRenderer();
    _audioManager     = new AudioManager();
    _deferredFBO      = new DeferredFrameBuffer();
    _bloom            = new Bloom();
    _ssaoPass         = new SSAO();
    _water            = new Water(_viewManager->getEventWrapper());
    _add              = new SSCompute("add", 
                                      IOEventDistributor::screenPixelWidth,
                                      IOEventDistributor::screenPixelHeight,
                                      TextureFormat::RGBA_UNSIGNED_BYTE);

    _mergeShader      = static_cast<MergeShader*>(ShaderBroker::instance()->getShader("mergeShader"));
    //_environmentMap  = new EnvironmentMap(2000, 2000);

    if (_graphicsLayer == GraphicsLayer::OPENGL) {
        _terminal = Terminal::instance();
        _terminal->initTerminal(_deferredRenderer->getGBuffers(), _entityList);

        if (_generatorMode) {
            // Use tile generator to create a randomly generated scene based on a rules system
            _pathCounter = 0;
        }
    }

    Vector4 sunLocation(0.0f, 0.0f, 700.0f);

    MVP lightMapMVP;
    lightMapMVP.setView(Matrix::translation(sunLocation.getx(), sunLocation.gety(), sunLocation.getz()) *
                        Matrix::rotationAroundX(45.0f));
    lightMapMVP.setProjection(Matrix::ortho(1400.0f, 1400.0f, 0.0f, 1400.0f));
    _lightList.push_back(new ShadowedDirectionalLight(_viewManager->getEventWrapper(),
                                                      lightMapMVP,
                                                      EffectType::None,
                                                      Vector4(1.0, 0.0, 0.0)));

    ////Model view projection matrix for point light additions
    //MVP pointLightMVP;

    ////point light projection has a 90 degree view angle with a 1 aspect ratio for generating square shadow maps
    ////with a near z value of 1 and far z value of 100
    //pointLightMVP.setProjection(Matrix::projection(90.0f, 1.0f, 1.0f, 100.0f));

    //pointLightMVP.setModel(Matrix::translation(198.45f, 24.68f, 186.71f));
    //_lightList.push_back(new Light(_viewManager->getEventWrapper(),
    //    pointLightMVP,
    //    LightType::POINT,
    //    EffectType::Fire,
    //    Vector4(1.0f, 0.8f, 0.3f, 1.0f)));

    //pointLightMVP.setModel(Matrix::translation(178.45f, 143.59f, 240.71f));
    //_lightList.push_back(new Light(_viewManager->getEventWrapper(),
    //    pointLightMVP,
    //    LightType::POINT,
    //    EffectType::Smoke,
    //    Vector4(0.4f, 0.4f, 0.4f, 1.0f)));

    //pointLightMVP.setModel(Matrix::translation(0.0f, 10.0f, 0.0f));
    //_lightList.push_back(new Light(_viewManager->getEventWrapper(),
    //    pointLightMVP,
    //    LightType::POINT,
    //    EffectType::Fire,
    //    Vector4(1.0f, 0.8f, 0.3f, 1.0f)));

    if (_graphicsLayer >= GraphicsLayer::DX12) {

        //_terminal = new Terminal(_deferredRenderer->getGBuffers(), _entityList);
        auto dxLayer = DXLayer::instance();
        dxLayer->fenceCommandList();

        if (_useRaytracing) {

            dxLayer->initCmdLists();

            _rayTracingPipeline = new RayTracingPipelineShader("rayTracingUberShader.hlsl",
                                                               DXLayer::instance()->getDevice(),
                                                               DXGI_FORMAT_R8G8B8A8_UNORM,
                                                               _entityList);
            dxLayer->fenceCommandList();
        }
    }

    //_audioManager->startAll();
    MasterClock::instance()->run(); //Scene manager kicks off the clock event manager
    _viewManager->triggerEvents();
    _viewManager->setEntityList(_entityList);

    //_physics = new Physics();
    //Gives physics a pointer to all models which allows access to underlying geometry
    //_physics->addEntities(_entityList); 
    //_physics->run(); //Dispatch physics to start kinematics

    _inputLayer->run();
}

EngineManager::~EngineManager() {
    for (auto entity : _entityList) {
        delete entity;
    }
    delete _deferredRenderer;
    delete _viewManager;
    //delete _audioManager;
    delete _forwardRenderer;
}

void EngineManager::removeEntity(Entity* entity) {
    _entityListLock.lock();
    int indexToRemove = 0;
    for (auto ent : _entityList) {
        if (ent == entity) {
            break;
        }
        indexToRemove++;
    }
    _entityList.erase(_entityList.begin() + indexToRemove);
    entity->setPosition(Vector4(100000000.0f,
                                100000000.0f,
                                100000000.0f,
                                1.0f));
    _entityListLock.unlock();
}

Entity* EngineManager::addEntity(Model* model, Matrix transform, bool temporaryModel) {
    auto viewManager = ModelBroker::getViewManager();
    auto viewWrapper = viewManager->getEventWrapper();
    
    MVP mvp;
    mvp.setModel(transform);
    mvp.setView(viewManager->getView());
    mvp.setProjection(viewManager->getProjection());

    if (temporaryModel) {

        if (_shadowEntity == nullptr) {
            _shadowEntity = new Entity(model, viewWrapper, mvp);

            _entityListLock.lock();
            _entityList.push_back(_shadowEntity); //Add a static model to the scene
            _entityListLock.unlock();
        }
        else {
            if (model->getName() == _shadowEntity->getModel()->getName()) {
                for (auto entity : _entityList) {
                    if (entity == _shadowEntity) {
                        entity->setMVP(mvp);
                    }
                }
            }
            else {
                _entityListLock.lock();

                std::vector<Entity*>::iterator it = _entityList.begin();
                Entity* entityToErase             = nullptr;
                for (auto entity : _entityList) {

                    if (entity == _shadowEntity) {
                        entityToErase = entity;
                        //First throw it to infinity lol
                        entity->setPosition(Vector4(100000000.0f,
                                                    100000000.0f,
                                                    100000000.0f,
                                                    1.0f));
                        break;
                    }
                    it++;
                }
                if (entityToErase != nullptr) {
                    _entityList.erase(it);
                }

                _entityList.push_back(new Entity(model,
                                                 viewWrapper,
                                                 mvp));

                _entityListLock.unlock();

                _shadowEntity = _entityList.back();
            }
        }
    }
    else {
        _entityListLock.lock();
        //Add an entity to the scene
        _entityList.push_back(new Entity(model,
                                         viewWrapper,
                                         mvp));
        _entityListLock.unlock();
    }
    return _entityList.back();
}

GraphicsLayer EngineManager::getGraphicsLayer() {
    return _graphicsLayer;
}

std::vector<Entity*>* EngineManager::getEntityList() {
    return &_entityList;
}

RayTracingPipelineShader* EngineManager::getRTPipeline() {
    return _rayTracingPipeline;
}

void EngineManager::_preDraw() {

    //Init command lists
    if (_graphicsLayer >= GraphicsLayer::DX12) {
        DXLayer::instance()->initCmdLists();
    }

    if (_viewManager->getViewState() == Camera::ViewState::POINT_SHADOW      ||
        _viewManager->getViewState() == Camera::ViewState::DEFERRED_LIGHTING ||
        _viewManager->getViewState() == Camera::ViewState::CAMERA_SHADOW     ||
        _viewManager->getViewState() == Camera::ViewState::MAP_SHADOW) {
       
        //send all vbo data to point light shadow pre pass
        for (Light* light : _lightList) {
            //ray trace the second direcional light
            if (_useRaytracing == false || light == _lightList[0]) {
                light->renderShadow(_entityList);
            }
        }
    }

    if (_graphicsLayer == GraphicsLayer::DXR) {

        if (_useRaytracing) {
            _rayTracingPipeline->doRayTracing(_entityList[0], _lightList[0]);
            //DXLayer::instance()->present(_rayTracingPipeline->getRayTracingTarget());

            auto depthTexture = static_cast<RenderTexture*>((
                                static_cast<ShadowedDirectionalLight*>(_lightList[0]))->getDepthTexture());

            HLSLShader::setOM({ *depthTexture },
                              depthTexture->getWidth(),
                              depthTexture->getHeight());

            auto depthBlit = static_cast<BlitDepthShader*>(ShaderBroker::instance()->getShader("blitDepthShader"));

            depthBlit->runShader(_rayTracingPipeline->getRayTracingTarget(),
                                 depthTexture);

            HLSLShader::releaseOM({ *depthTexture });
        }
    }

    //Establish an offscreen Frame Buffer Object to generate G buffers for deferred shading
    _deferredRenderer->bind();
}
void EngineManager::_postDraw() {

    if (_graphicsLayer == GraphicsLayer::OPENGL) {

        glCheck();
        //Render the water around the island
        //_water->render();

        //unbind fbo
        _deferredRenderer->unbind();


        if ((_generatorMode == true) &&
            ((++_pathCounter) % 1 == 0)) {

            WorldGenerator::spawnPaths("SPAWN-TEST");
        }

        if (_viewManager->getViewState() == Camera::ViewState::DEFERRED_LIGHTING) {

            //Only compute ssao for opaque objects
            _ssaoPass->computeSSAO(_deferredRenderer->getGBuffers(),
                                   _viewManager);

            //Bind frame buffer
            glBindFramebuffer(GL_FRAMEBUFFER,
                              _deferredFBO->getFrameBufferContext());

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //Pass lights to deferred shading pass
            _deferredRenderer->deferredLighting(_lightList,
                                                _viewManager,
                                                _ssaoPass,
                                                _environmentMap);

            //Draw transparent objects onto of the deferred renderer
            _forwardRenderer->forwardLighting(_entityList,
                                              _viewManager,
                                              _lightList);

            // Lights - including the fire point lights
            for (Light* light : _lightList) {
                light->render();

                for (Entity* entity : _entityList) {
                    
                    if (light->getType() == LightType::SHADOWED_DIRECTIONAL) {
                        
                        Matrix inverseViewProjection = ModelBroker::getViewManager()->getView().inverse() *
                                                       ModelBroker::getViewManager()->getProjection().inverse();

                        //FrustumCuller::getVisibleOBB(entity, inverseViewProjection, light);
                    }
                }
            }

            //_viewManager->displayViewFrustum();

            for (auto entity : _entityList) {
                if (entity->getSelected()) {
                    //entity->getFrustumCuller()->visualize();
                }
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            //Compute bloom from deferred fbo texture
            _bloom->compute(_deferredFBO->getRenderTexture());

            //If adding a second texture then all writes are to this texture second param
            _add->compute(_deferredFBO->getRenderTexture(),
                          _bloom->getTexture());

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            Texture* velocityTexture = &_deferredRenderer->getGBuffers()->getTextures()[2];
            _mergeShader->runShader(_bloom->getTexture(),
                                    velocityTexture);
        }
        else if (_viewManager->getViewState() == Camera::ViewState::PHYSICS) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            _physics->visualize();
            //_entityList[1]->getFrustumCuller()->visualize();
            _viewManager->displayViewFrustum();

            //shows all of the light/shadow volumes
            for (Light* light : _lightList) {
                light->renderDebug();
            }
        }
        else {

            //Only compute ssao for opaque objects
            _ssaoPass->computeSSAO(_deferredRenderer->getGBuffers(),
                                   _viewManager);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            //Pass lights to deferred shading pass
            _deferredRenderer->deferredLighting(_lightList,
                                                _viewManager,
                                                _ssaoPass,
                                                _environmentMap);
        }

        _terminal->processCommands();
    }
    else {

        //unbind fbo
        _deferredRenderer->unbind();

        //Only compute ssao for opaque objects
        _ssaoPass->computeSSAO(_deferredRenderer->getGBuffers(),
            _viewManager);

        RenderTexture* renderTexture = static_cast<RenderTexture*>(_deferredFBO->getRenderTexture());
        RenderTexture* depthTexture = static_cast<RenderTexture*>(_deferredFBO->getDepthTexture());
        RenderTexture* debug0Texture = static_cast<RenderTexture*>(_deferredFBO->getDebug0Texture());
        RenderTexture* debug1Texture = static_cast<RenderTexture*>(_deferredFBO->getDebug1Texture());

        HLSLShader::setOM({ *renderTexture, *debug0Texture, *debug1Texture, *depthTexture },
                          IOEventDistributor::screenPixelWidth,
                          IOEventDistributor::screenPixelHeight);

        if (_viewManager->getViewState() == Camera::ViewState::DEFERRED_LIGHTING) {


            //Pass lights to deferred shading pass
            _deferredRenderer->deferredLighting(_lightList,
                _viewManager,
                _ssaoPass,
                nullptr);

            _forwardRenderer->forwardLighting(_entityList,
                _viewManager,
                _lightList);

            // Lights - including the fire point lights
            for (Light* light : _lightList) {
                light->render();
            }

            HLSLShader::releaseOM({ *renderTexture, *debug0Texture, *debug1Texture, *depthTexture });

            //Compute bloom from deferred fbo texture
            _bloom->compute(_deferredFBO->getRenderTexture());

            //If adding a second texture then all writes are to this texture second param
            _add->compute(_deferredFBO->getRenderTexture(), _bloom->getTexture());
            _add->uavBarrier();

            HLSLShader::setOM({ *renderTexture, *debug0Texture, *debug1Texture, *depthTexture },
                              IOEventDistributor::screenPixelWidth,
                              IOEventDistributor::screenPixelHeight);

            Texture* velocityTexture = &_deferredRenderer->getGBuffers()->getTextures()[2];
            _mergeShader->runShader(_bloom->getTexture(), velocityTexture);

            HLSLShader::releaseOM({ *renderTexture, *debug0Texture, *debug1Texture, *depthTexture });

            DXLayer::instance()->present(_deferredFBO->getRenderTexture());
        }
        else
        {

            //Pass lights to deferred shading pass
            _deferredRenderer->deferredLighting(_lightList,
                                                _viewManager,
                                                _ssaoPass,
                                                nullptr);

            HLSLShader::releaseOM({ *renderTexture, *debug0Texture, *debug1Texture, *depthTexture });

            DXLayer::instance()->present(_deferredFBO->getRenderTexture());

        }
    }
}