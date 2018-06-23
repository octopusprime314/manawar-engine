#include "ProcIsland.h"
#include "ViewManager.h"
#include "Factory.h"
#include "Model.h"
#include "Noise.h"
#include "InstancedForwardShader.h"

#include <cassert>

// Affine transform from [0, 1] to [-10, 8.5]
static float ScaleNoiseToTerrainHeight(float noise)
{
    constexpr float scale = 5.f;
    constexpr float max = 1.7f * scale;
    constexpr float min = -2.f * scale;

    // TODO: We may want to make this nonlinear.
    return (max - min) * noise + min;
}

static Model* GenerateTerrain()
{
    // Generate geometry
    constexpr int minX = 80;
    constexpr int maxX = 95;
    constexpr int minZ = 80;
    constexpr int maxZ = 110;

    constexpr unsigned dX = maxX - minX;
    constexpr unsigned dZ = maxZ - minZ;
    constexpr float delta = 0.2f;
    constexpr unsigned stride = unsigned(dX / delta + 0.5f) + 1;

    static_assert(minX <= maxX, "Check X min/max bounds");
    static_assert(minZ <= maxZ, "Check Z min/max bounds");
    static_assert(delta > 0.f, "Check your delta");

    // Generate Vertices. We'll do indices and normals after.
    std::vector<Vector4> verts;
    verts.reserve(static_cast<int>(((maxX - minX) / delta + 1.f) *
        ((maxZ - minZ) / delta + 1.f)));
    for (float z = minZ; z <= maxZ; z += delta) {
        for (float x = minX; x <= maxX; x += delta) {
            float y = ScaleNoiseToTerrainHeight(kNoise.turbulence(2500.f*x / 150.f, 3250.f*z / 150 + 400, 9));
            verts.emplace_back(x - minX - dX / 2, y, z - minZ - dZ / 2, 1.f);
        }
    }

    // Populate Indices
    std::vector<int> indices;

    for (int vertIdx = 0; (vertIdx + 1 + stride) < verts.size(); vertIdx += 1) {
        int b = vertIdx;
        if ((b + 1) % stride == 0) {
            continue;
        }
        indices.emplace_back(b + stride);   assert((b + stride) < verts.size());
        indices.emplace_back(b + 1);        assert((b + 1) < verts.size());
        indices.emplace_back(b + 0);        assert((b + 0) < verts.size());

        indices.emplace_back(b + stride);   assert((b + stride) < verts.size());
        indices.emplace_back(b + 1 + stride); assert((b + 1 + stride) < verts.size());
        indices.emplace_back(b + 1);        assert((b + 1) < verts.size());
    }

    // Populate Normals
    std::vector<Vector4> normals(verts.size(), Vector4(0.f, 0.f, 0.f, 0.f));
    for (int i = 0; i + 2 < indices.size(); i += 3) {
        Vector4 vert0 = verts[indices[i + 0]];
        Vector4 vert1 = verts[indices[i + 1]];
        Vector4 vert2 = verts[indices[i + 2]];

        Vector4 a = vert0 - vert1;
        Vector4 b = vert0 - vert2;
        Vector4 normal = a.crossProduct(b);
        assert(normal.getMagnitude() > 1e-6f);
        assert(normal.gety() > 0.f);
        normal.normalize();

        normals[indices[i + 0]] += normal;
        normals[indices[i + 1]] += normal;
        normals[indices[i + 2]] += normal;
    }
    // Normalize at the end, since any index could potentially modify any normal.
    for (Vector4& normal : normals) {
        normal.normalize();
    }

    // Populate texture coordiantes
    std::vector<Tex2> texs;
    texs.reserve(verts.size());
    for (int i = 0; i < verts.size(); i += 1) {
        Vector4 vert = verts[i];
        texs.emplace_back(vert.getx(), vert.gety());
    }

    printf("Terrain2 Index Count:    %zu\n", indices.size());
    printf("Terrain2 Triangle Count: %zu\n", 3 * indices.size());
    printf("Terrain2 Vertex Count:   %zu\n", verts.size());

    // indices.size() is invalid after we move out of it, so save the length before hand.
    int textureStride = static_cast<int>(indices.size());

    RenderBuffers renderBuffers;
    *renderBuffers.getVertices() = std::move(verts);
    *renderBuffers.getIndices() = std::move(indices);
    *renderBuffers.getNormals() = std::move(normals);
    *renderBuffers.getTextures() = std::move(texs);
    // This is a relatively expensive copy, and I don't think we need it. (???)
    // *renderBuffers.getDebugNormals() = *renderBuffers.getNormals();

    Model* pModel = new Model(Factory::_viewEventWrapper,
        std::move(renderBuffers),
        new StaticShader("staticTerrainShader"));
    // We use this texture for its strides - the actualy texture loaded doesn't matter.
    // ***THIS MUST NOT HAVE AN ALPHA CHANNEL!***.
    // The existance of an alpha channel triggers extra functionality that we do not want.
    pModel->addTexture("../assets/textures/landscape/Rock_6_d.png", textureStride);
    return pModel;
}

static Model* GenerateTrees()
{
    //Load tree model and instance it
    Model* treeModel = new Model("tree/tree.fbx", Factory::_viewEventWrapper, ModelClass::ModelType);

    treeModel->getMVP()->setModel(Matrix::scale(0.01f));

    //Generate instancing offsets
    constexpr int minX = 80;
    constexpr int maxX = 95;
    constexpr int minZ = 80;
    constexpr int maxZ = 110;

    constexpr float trunkSize = 0.012f;
    constexpr float treeHeight = trunkSize * 20.f;

    constexpr float delta = trunkSize * 100.f;
    constexpr int deltaX = maxX - minX + 1;
    constexpr int deltaZ = maxZ - minZ + 1;

    std::vector<Vector4> offsets;
    for (float x = minX; x <= maxX; x += delta) {
        for (float z = minZ; z <= maxZ; z += delta) {
            float y = ScaleNoiseToTerrainHeight(kNoise.turbulence(2500.f*x / 150.f, 3250.f*z / 150 + 400, 9));
            // Roughly the green area in staticTerrainShader.frag, less some at the top.
            if (!(0.25 < y && y < 1.6)) {
                continue;
            }
            float offsetX = x - ((110 - 50) / 2.f + 50 + (maxX - minX) / 2);
            float offsetZ = z - ((110 - 50) / 2.f + 50 + (maxZ - minZ) / 2);
            offsets.push_back(Vector4(offsetX, y, offsetZ, 0.0));
        }
    }

    treeModel->setInstances(offsets);
    return treeModel;
}

void GenerateProceduralIsland(std::vector<Model*>& models, ProcState params)
{
    Model* pTerrain = GenerateTerrain();
    models.push_back(pTerrain);
    Model* pTrees = GenerateTrees();
    models.push_back(pTrees);

    //Add textures for procedural island
    TextureBroker* texBroker = TextureBroker::instance();
    texBroker->addTexture("../assets/textures/landscape/grass.jpg");
    texBroker->addTexture("../assets/textures/landscape/snow.jpg");
    texBroker->addTexture("../assets/textures/landscape/dirt.jpg");
    texBroker->addTexture("../assets/textures/landscape/rocks.jpg");
}
