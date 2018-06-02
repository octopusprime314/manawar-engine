#include "ProcIsland.h"
#include "ViewManager.h"
#include "Factory.h"
#include "Model.h"
#include "Noise.h"

// Affine transform from [0, 1] to [-10, 8.5]
static float ScaleNoiseToTerrainHeight(float noise)
{
    constexpr float scale = 5.f;
    constexpr float max = 1.7f * scale;
    constexpr float min = -2.f * scale;

    // TODO: We may want to make this nonlinear.
    return (max - min) * noise + min;
}

static Model* makeModelFromTriangles(std::vector<Triangle>& triangles, StaticShader* pShader, const char* pName)
{
    RenderBuffers renderBuffers;
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

        // Indices
        int indexBase = static_cast<int>(verts.size());
        indices.emplace_back(indexBase);
        indices.emplace_back(indexBase + 1);
        indices.emplace_back(indexBase + 2);

        // Positions
        verts.emplace_back(positions[0]);
        verts.emplace_back(positions[1]);
        verts.emplace_back(positions[2]);

        // Normals - computed
        Vector4 a = positions[0] - positions[1];
        Vector4 b = positions[0] - positions[2];
        Vector4 normal = a.crossProduct(b);
        normal.normalize();
        normals.emplace_back(normal);
        normals.emplace_back(normal);
        normals.emplace_back(normal);

        // Texture Coords
        // Use height as text coords.
        texs.emplace_back(positions[0].getx(), positions[0].gety());
        texs.emplace_back(positions[1].getx(), positions[1].gety());
        texs.emplace_back(positions[2].getx(), positions[2].gety());
    }
    *renderBuffers.getDebugNormals() = normals;

    printf("%s Index Count:    %zu\n", pName, indices.size());
    printf("%s Triangle Count: %zu\n", pName, triangles.size());
    printf("%s Vertex Count:   %zu\n", pName, 3 * triangles.size());

    if (Factory::_viewEventWrapper == nullptr) {
        printf("Why is Factory::_viewEventWrapper null?\n");
    }
    Model* pModel = new Model(Factory::_viewEventWrapper, std::move(renderBuffers), pShader);
    // We use this texture for its strides - the actualy texture loaded doesn't matter.
    // Just make sure it ***DOES NOT HAVE AN ALPHA CHANNEL***.
    pModel->addTexture("../assets/textures/landscape/Rock_6_d.png",
                           static_cast<int>(indices.size()));
    return pModel;
}

static Model* GenerateTerrain()
{
    std::vector<Triangle> triangles;

    constexpr int minX = 80;
    constexpr int maxX = 95;
    constexpr int minZ = 80;
    constexpr int maxZ = 110;
    constexpr float delta = 0.2f;

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
                                   base + Vector4(0.f,   0.f, delta),
                                   base + Vector4(delta, 0.f, delta));

            triangles.emplace_back(base + Vector4(0.f,   0.f, 0.f),
                                   base + Vector4(delta, 0.f, delta),
                                   base + Vector4(delta, 0.f, 0.f));
        }
    }

    std::vector<size_t> toDelete;

    for (size_t idx = 0; idx < triangles.size(); idx += 1) {
        auto& triangle = triangles[idx];
        int votesToCull = 0;
        for (auto& point : triangle.getTrianglePoints()) {
            auto& x = point.getFlatBuffer()[0];
            auto& y = point.getFlatBuffer()[1];
            auto& z = point.getFlatBuffer()[2];
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
            y = ScaleNoiseToTerrainHeight(kNoise.turbulence(2500.f*x / 150.f, 3250.f*z / 150 + 400, 9));
            if (y < 0.f) {
                votesToCull += 1;
            }

            {
                // Center everything around the origin.
                x -= (110-50) / 2.f + 50 + (maxX - minX)/2;
                z -= (110-50) / 2.f + 50 + (maxZ - minZ)/2;
            }
        }
        if (votesToCull != 0) {
            toDelete.emplace_back(idx);
        }
    }

    printf("Found %zu terrain triangles to cull\n", toDelete.size());
    std::vector<Triangle> trianglesAfterCull;
    trianglesAfterCull.reserve(triangles.size());
    size_t toDeleteIdx = 0;
    for (size_t idx = 0; idx < triangles.size(); idx += 1) {
        if (toDeleteIdx < toDelete.size() && idx == toDelete[toDeleteIdx]) {
            toDeleteIdx += 1;
            continue;
        }
        trianglesAfterCull.emplace_back(triangles[idx]);
    }
    return makeModelFromTriangles(trianglesAfterCull, new StaticShader("staticTerrainShader"), "Island Terrain");
}

static Model* GenerateTrees()
{
    std::vector<Triangle> triangles;
    constexpr int minX = 80;
    constexpr int maxX = 95;
    constexpr int minZ = 80;
    constexpr int maxZ = 110;

    constexpr float trunkSize = 0.012f;
    constexpr float treeHeight = trunkSize * 20.f;

    constexpr float delta = trunkSize * 10.f;

    static_assert(minX <= maxX, "Check X min/max bounds");
    static_assert(minZ <= maxZ, "Check Z min/max bounds");
    static_assert(delta > 0.f, "Check your delta");

    // 4 triangles per "tree" * (min-max and zero) ish
    {
        constexpr int trisPerUnit = static_cast<int>(1.f / delta);
        constexpr int deltaX = maxX - minX + 1;
        constexpr int deltaZ = maxZ - minZ + 1;
        triangles.reserve(4 * deltaX * deltaZ * trisPerUnit * trisPerUnit);
    }
    for (float x = minX; x <= maxX; x += delta) {
        for (float z = minZ; z <= maxZ; z += delta) {
            float y = ScaleNoiseToTerrainHeight(kNoise.turbulence(2500.f*x / 150.f, 3250.f*z / 150 + 400, 9));
            // Roughly the green area in staticTerrainShader.frag, less some at the top.
            if (!(0.25 < y && y < 1.6)) {
                continue;
            }
            Vector4 base = Vector4(x, y, z, 1.f);
            auto top = Vector4( 0.f, treeHeight,  0.f)       + base;
            auto v00 = Vector4(-trunkSize,  0.f, -trunkSize) + base;
            auto v01 = Vector4(-trunkSize,  0.f,  trunkSize) + base;
            auto v10 = Vector4( trunkSize,  0.f, -trunkSize) + base;
            auto v11 = Vector4( trunkSize,  0.f,  trunkSize) + base;

            triangles.emplace_back(v00, top, v10);
            triangles.emplace_back(v10, top, v11);
            triangles.emplace_back(v11, top, v01);
            triangles.emplace_back(v01, top, v00);
        }
    }

    for (auto& triangle : triangles) {
        auto& positions = triangle.getTrianglePoints();
        for (auto& position : positions) {
            // Center everything around the origin.
            float& x = position.getFlatBuffer()[0];
            float& z = position.getFlatBuffer()[2];
            float X = (maxX - minX) / 2;
            float Z = (maxZ - minZ) / 2;
            x -= (110 - 50) / 2.f + 50 + (maxX - minX) / 2;
            z -= (110 - 50) / 2.f + 50 + (maxZ - minZ) / 2;
        }
    }

    // Mega Tree, centered at the origin
    {
        constexpr float treeHeight = 10.f;
        constexpr float trunkSize = 1.f;
        Vector4 base = Vector4(0, 0, 0, 1.f);
        auto top = Vector4(0.f, treeHeight, 0.f) + base;
        auto v00 = Vector4(-trunkSize, 0.f, -trunkSize) + base;
        auto v01 = Vector4(-trunkSize, 0.f, trunkSize) + base;
        auto v10 = Vector4(trunkSize, 0.f, -trunkSize) + base;
        auto v11 = Vector4(trunkSize, 0.f, trunkSize) + base;

        triangles.emplace_back(v00, top, v10);
        triangles.emplace_back(v10, top, v11);
        triangles.emplace_back(v11, top, v01);
        triangles.emplace_back(v01, top, v00);
    }

    return makeModelFromTriangles(triangles, new StaticShader("staticShader"), "Island Trees");
}

void GenerateProceduralIsland(std::vector<Model*>& models, ProcState params)
{
    Model* pTerrain = GenerateTerrain();
    models.push_back(pTerrain);
    Model* pTrees = GenerateTrees();
    models.push_back(pTrees);
}
