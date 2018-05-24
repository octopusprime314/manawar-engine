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

// Affine transform from [0, 1] to [-10, 8.5]
float ScaleNoiseToTerrainHeight(float noise)
{
    constexpr float scale = 5.f;
    constexpr float max = 1.7f * scale;
    constexpr float min = -2.f * scale;

    // TODO: We may want to make this nonlinear.
    return (max - min) * noise + min;
}

// TODO: Put this somewhere else.
Model* GenerateLandscape()
{
    RenderBuffers renderBuffers;

    // Generate geometry
    std::vector<Triangle> triangles;
    {
        constexpr int S = 150;
        constexpr int minX = 50;
        constexpr int maxX = 110;
        constexpr int minZ = 50;
        constexpr int maxZ = 110;
        constexpr float delta = 0.1f;

        static_assert(minX <= maxX, "Check X min/max bounds");
        static_assert(minZ <= maxZ, "Check Z min/max bounds");
        static_assert(delta > 0.f,  "Check your delta");

        // 2 triangles per quad * (min-max and zero) ish
        {
            constexpr int trisPerUnit = static_cast<int>(1.f / delta);
            constexpr int deltaX = maxX - minX + 1;
            constexpr int deltaZ = maxZ - minZ + 1;
            triangles.reserve(2 * deltaX * deltaZ * trisPerUnit * trisPerUnit);
        }
        for (float x = minX; x <= maxX; x += delta) {
            for (float z = minZ; z <= maxZ; z += delta) {
                Vector4 base = Vector4((float)x, 0.f, (float)z, 1.f);

                // Insert quads as pairs of triangles
                triangles.emplace_back(base + Vector4(0.f,   0.f, 0.f),
                                       base + Vector4(delta, 0.f, 0.f),
                                       base + Vector4(delta, 0.f, delta));

                triangles.emplace_back(base + Vector4(delta, 0.f, delta),
                                       base + Vector4(0.f,   0.f, delta),
                                       base + Vector4(0.f,   0.f, 0.f));
            }
        }

        for (auto& triangle : triangles) {
            auto& positions = triangle.getTrianglePoints();
            for (auto& position : positions) {
                auto& x = position.getFlatBuffer()[0];
                auto& y = position.getFlatBuffer()[1];
                auto& z = position.getFlatBuffer()[2];
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
                y = ScaleNoiseToTerrainHeight(kNoise.turbulence(2500.f*x / S, 3250.f*z / S + 400, 9));
                // Center everything around the origin.
                x -= (maxX - minX) / 2.f + minX;
                z -= (maxZ - minZ) / 2.f + minZ;
            }
        }
    }
    size_t reserveSize = 3 * triangles.size();

    std::vector<Vector4>& verts = *renderBuffers.getVertices();
    verts.reserve(reserveSize);

    std::vector<int>& indices = *renderBuffers.getIndices();
    indices.reserve(reserveSize);

    std::vector<Vector4>& normals = *renderBuffers.getNormals();
    normals.reserve(reserveSize);

    std::vector<Tex2>& texs = *renderBuffers.getTextures();
    texs.reserve(reserveSize);


    for (auto& triangle : triangles) {
        auto& positions = triangle.getTrianglePoints();

        // We can cull however we want here!

        // Cull "under water" triangles. This helps more than you'd expect.
        if (positions[0].gety() < -1.f &&
            positions[1].gety() < -1.f &&
            positions[2].gety() < -1.f)
        {
            continue;
        }

        // Indices
        int indexBase = static_cast<int>(verts.size());
        indices.emplace_back(indexBase);
        indices.emplace_back(indexBase + 1);
        indices.emplace_back(indexBase + 1);

        // Positions
        verts.emplace_back(positions[0]);
        verts.emplace_back(positions[1]);
        verts.emplace_back(positions[2]);

        // Normals - computed
        Vector4 normal = positions[0].crossProduct(positions[1]);
        if (normal.gety() < 0.f) {
            // Force them "up"
            float* n = normal.getFlatBuffer();
            n[0] = -n[0];
            n[1] = -n[1];
            n[2] = -n[2];
        }
        normal.normalize();
        normals.emplace_back(normal);
        normals.emplace_back(normal);
        normals.emplace_back(normal);

        // Texture Coords
        // Use height as text coords.
        texs.emplace_back(positions[0].gety(), 0.f);
        texs.emplace_back(positions[1].gety(), 0.f);
        texs.emplace_back(positions[2].gety(), 0.f);
    }
    *renderBuffers.getDebugNormals() = normals;

    printf("Terrain Index Count:    %zu\n", indices.size());
    printf("        Triangle Count: %zu\n", triangles.size());
    printf("        Vertex Count:   %zu\n", 3 * triangles.size());

    Model* pLandscape = new Model(Factory::_viewEventWrapper, std::move(renderBuffers), new StaticShader("staticShader"));
    pLandscape->addTexture("../assets/textures/landscape/sunbeams01.dds",
                           static_cast<int>(indices.size()));
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

    Model* pLandscape = GenerateLandscape();
    _modelList.push_back(pLandscape);

    _viewManager->setProjection(viewportWidth, viewportHeight, nearPlaneDistance, farPlaneDistance); //Initializes projection matrix and broadcasts upate to all listeners
    // This view is carefully chosen to look at a mountain without showing the (lack of) water in the scene.
    _viewManager->setView(Matrix::cameraTranslation(0.f, 0.68f, 20.f),
                          Matrix::cameraRotationAroundY(-45.f),
                          Matrix());
    _viewManager->setModelList(_modelList);

    //_physics.addModels(_modelList); //Gives physics a pointer to all models which allows access to underlying geometry
    //_physics.run(); //Dispatch physics to start kinematics

    //Add a directional light pointing down in the negative y axis
    MVP lightMVP;
    lightMVP.setView(Matrix::cameraTranslation(-50.0f, -50.0f, 0) * Matrix::cameraRotationAroundX(-90.0f));
    lightMVP.setProjection(Matrix::cameraOrtho(100, 100, 1.0f, 1e3));
    _lightList.push_back(Factory::make<Light>(lightMVP, LightType::CAMERA_DIRECTIONAL));

    MVP lightMapMVP;
    lightMapMVP.setView(Matrix::cameraTranslation(-50.0f, -50.0f, 0) * Matrix::cameraRotationAroundX(-90.0f));
    lightMapMVP.setProjection(Matrix::cameraOrtho(100, 100, 1.0f, 1e3));
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
