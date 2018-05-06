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

#include <Noise.h>
#include <Triangle.h>

// TODO: Put this somewhere else.
Model* GenerateLandscape()
{
    // Generate geometry
    std::vector<Triangle> triangles;
    {
        constexpr int S = 100;
        constexpr int minX = 0;
        constexpr int maxX = +S;
        constexpr int minZ = 0;
        constexpr int maxZ = +S;

        static_assert(minX <= maxX, "Check X min/max bounds");
        static_assert(minZ <= maxZ, "Check Z min/max bounds");

        // 2 triangles per quad * (min-max and zero)
        triangles.reserve(2 * (maxX-minX + 1) * (maxZ-minZ + 1));
        for (int x = minX; x <= maxX; x += 1) {
            for (int z = minZ; z <= maxZ; z += 1) {
                Vector4 base = Vector4((float)x, 0, (float)z, 1.f);
                auto tri1 = Triangle(base + Vector4(0.f, 0.f, 0.f, 1.f),
                                     base + Vector4(1.f, 0.f, 0.f, 1.f),
                                     base + Vector4(1.f, 0.f, 1.f, 1.f)
                );
                auto tri2 = Triangle(base + Vector4(1.f, 0.f, 1.f, 1.f),
                                     base + Vector4(0.f, 0.f, 1.f, 1.f),
                                     base + Vector4(0.f, 0.f, 0.f, 1.f)
                );
                // Insert quads as pairs of triangles
                triangles.emplace_back(tri1);
                triangles.emplace_back(tri2);
            }
        }

        for (auto& triangle : triangles) {
            auto& positions = triangle.getTrianglePoints();
            for (auto& position : positions) {
                auto& x = position.getFlatBuffer()[0];
                auto& y = position.getFlatBuffer()[1];
                auto& z = position.getFlatBuffer()[2];
                constexpr float scale = 15.f;
                constexpr float max   = 1.7f * scale;
                constexpr float min   = -2.f * scale;
                // We want to color things with bounds like this. Ish.
                // Scale 'start' by 'scale' first.
                //  gradient = [
                //       Start   Color
                //      (-1.000, (0, 0, 128)),        # deep water
                //      (-0.150, (0, 0, 255)),        # shallow water
                //      (-0.050, (0, 128, 255)),      # shore
                //      (0.000,  (0xFF, 0xEC, 0x96)), # sand
                //      (0.060,  (32, 160, 0)),       # grass
                //      (0.400,  (128, 128, 128)),    # rock
                //      (0.500,  (96, 96, 96)),       # rock
                //      (0.600,  (255, 255, 255)),    # snow
                //  ]
                y = (max - min) * kNoise.turbulence(2500.f*x / S, 3250.f*z / S + 400, 9) + min;
                // Make "water" visible more easily for now.
                if (y < 0.f) {
                    y = -1;
                }
            }
        }
    }

    Model* pLandscape = new Model();

    // These should probably go in a default constructor for Model...
    pLandscape->_fbxLoader = nullptr;
    pLandscape->_clock = MasterClock::instance();
    pLandscape->_classId = ModelClass::ModelType;
    pLandscape->_debugMode = false;
    pLandscape->_debugShaderProgram = new DebugShader("debugShader");

    std::vector<Vector4>& verts = *pLandscape->_renderBuffers.getVertices();
    std::vector<Vector4>& normals = *pLandscape->_renderBuffers.getNormals();
    std::vector<Tex2>& texs = *pLandscape->_renderBuffers.getTextures();
    for (auto& triangle : triangles) {
        auto& positions = triangle.getTrianglePoints();
        verts.emplace_back(positions[0]);
        verts.emplace_back(positions[1]);
        verts.emplace_back(positions[2]);

        Vector4 normal = positions[0].crossProduct(positions[1]);
        normal.normalize();
        normals.emplace_back(normal);
        normals.emplace_back(normal);
        normals.emplace_back(normal);

        // Use location as text coords. We may want to scale this.
        // Eventually, we want the height to color terrain?
        texs.emplace_back(positions[0].getx(), positions[0].getz());
        texs.emplace_back(positions[1].getx(), positions[1].getz());
        texs.emplace_back(positions[2].getx(), positions[2].getz());
    }
    *pLandscape->_renderBuffers.getDebugNormals() = normals;
    std::vector<int>& indices = *pLandscape->_renderBuffers.getIndices();
    indices.reserve(verts.size());
    for (int i = 0; i < static_cast<int>(verts.size()); i += 1) {
        indices.emplace_back(i);
    }

    pLandscape->_vbo.createVBO(&pLandscape->_renderBuffers, pLandscape->_classId);
    pLandscape->_shaderProgram = new StaticShader("staticShader");
    pLandscape->_geometryType = GeometryType::Triangle;
    for (const auto& triangle : triangles) {
        pLandscape->_geometry.addTriangle(triangle);
    }
    pLandscape->addTexture("../assets/textures/landscape/sunbeams01.dds",
                           3 * static_cast<int>(triangles.size()));
    pLandscape->_clock->subscribeKinematicsRate(std::bind(&Model::_updateKinematics,
                                                          pLandscape,
                                                          std::placeholders::_1));
    return pLandscape;
}

SceneManager::SceneManager(int* argc, char** argv, unsigned int viewportWidth, unsigned int viewportHeight, float nearPlaneDistance, float farPlaneDistance) {

    _viewManager = new ViewManager(argc, argv, viewportWidth, viewportHeight);

    Factory::setViewWrapper(_viewManager); //Set the reference to the view model event interface

    _deferredRenderer = new DeferredRenderer();
    _forwardRenderer = new ForwardRenderer();

    _shadowRenderer = new ShadowRenderer(2000, 2000);

    _pointShadowMap = new PointShadowMap(2000, 2000);

    _ssaoPass = new SSAO();

    //_environmentMap = new EnvironmentMap(2000, 2000);

    _audioManager = new AudioManager();

    //Setup pre and post draw callback events received when a draw call is issued
    SimpleContextEvents::setPreDrawCallback(std::bind(&SceneManager::_preDraw, this));
    SimpleContextEvents::setPostDrawCallback(std::bind(&SceneManager::_postDraw, this));

    _modelList.push_back(GenerateLandscape());

    //int x = -900;
    //for (int i = 0; i < 1; ++i) {
    //    //_modelList.push_back(_modelFactory->makeAnimatedModel("troll/troll_idle.fbx")); //Add an animated model to the scene
    //    //_modelList.push_back(_modelFactory->makeAnimatedModel("hagraven/hagraven_idle.fbx")); //Add an animated model to the scene
    //    //_modelList.push_back(_modelFactory->makeAnimatedModel("wolf/wolf_turnleft.fbx")); //Add an animated model to the scene
    //    _modelList.push_back(Factory::make<AnimatedModel>("werewolf/werewolf_jump.fbx")); //Add an animated model to the scene

    //    //Simple kludge test to activate animated models in motion to stimulate collision detection tests
    //    _modelList.back()->getStateVector()->setActive(true);
    //    _modelList.back()->setPosition(Vector4(0.0f, 5.0f, -20.0f, 1.0f)); //Place objects 20 meters above sea level for collision testing
    //    x += 30;
    //}

    //_physics.addModels(_modelList); //Gives physics a pointer to all models which allows access to underlying geometry
    //_physics.run(); //Dispatch physics to start kinematics

    //Add a directional light pointing down in the negative y axis
    MVP lightMVP;
    lightMVP.setView(Matrix::cameraTranslation(0.0f, 0.0f, 100.0f) * Matrix::cameraRotationAroundX(-90.0f));
    lightMVP.setProjection(Matrix::cameraOrtho(200.0f, 200.0f, 1.0f, 200.0f));
    _lightList.push_back(Factory::make<Light>(lightMVP, LightType::CAMERA_DIRECTIONAL));

    MVP lightMapMVP;
    lightMapMVP.setView(Matrix::cameraTranslation(0.0f, 0.0f, 100.0f) * Matrix::cameraRotationAroundX(-90.0f));
    lightMapMVP.setProjection(Matrix::cameraOrtho(600.0f, 600.0f, 1.0f, 200.0f));
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

    _viewManager->setProjection(viewportWidth, viewportHeight, nearPlaneDistance, farPlaneDistance); //Initializes projection matrix and broadcasts upate to all listeners
    _viewManager->setView(Matrix::cameraTranslation(0.0, 2.0, -20.0), Matrix(), Matrix()); //Place view 25 meters in +z direction
    _viewManager->setModelList(_modelList);

    _audioManager->StartAll();

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
