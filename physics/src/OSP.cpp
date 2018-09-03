#include "OSP.h"
#include "GeometryMath.h"
#include "ModelBroker.h"
#include "ViewEventDistributor.h"

OSP::OSP(float cubicDimension, int maxGeometries) :
    _cubicDimension(cubicDimension),
    _maxGeometries(maxGeometries) {

}

OSP::~OSP() {

}

std::vector<OctNode<Cube*>*>* OSP::getOSPLeaves() {
    return &_ospLeaves;
}

std::vector<OctNode<Cube*>*>* OSP::getFrustumLeaves() {
    return &_frustumLeaves;
}

void OSP::generateGeometryOSP(std::vector<Entity*>& entities) {


    //Initialize a octary tree with a rectangle of cubicDimension located at the origin of the axis
    Cube* rootCube = new Cube(_cubicDimension, _cubicDimension, _cubicDimension, Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    OctNode<Cube*>* node = _octTree.insert(nullptr, rootCube);

    //Go through all of the models and populate
    for (auto entity : entities) {
        std::vector<Triangle>* triangles = entity->getGeometry()->getTriangles();

        for (Triangle & triangle : *triangles) {
            //if geometry data is contained within the first octet then build it out
            if (GeometryMath::triangleCubeDetection(&triangle, rootCube)) {

                node->addGeometry(entity, &triangle);
            }
        }

        std::vector<Sphere>* spheres = entity->getGeometry()->getSpheres();
        for (Sphere & sphere : *spheres) {

            //if geometry data is contained within the first octet then build it out
            if (GeometryMath::sphereCubeDetection(&sphere, rootCube)) {

                node->addGeometry(entity, &sphere);
            }
        }
    }

    //Recursively build Octary Space Partition Tree
    _buildOctetTree(_octTree.getRoot()->getData(), node);
}

void OSP::generateRenderOSP(Entity* entity) {


    //Initialize a octary tree with a rectangle of cubicDimension located at the origin of the axis
    Cube* rootCube = new Cube(_cubicDimension, _cubicDimension, _cubicDimension, Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    OctNode<Cube*>* node = _octTree.insert(nullptr, rootCube);

    auto triangleBuffer = entity->getModel()->getRenderBuffers();
    auto vertices = triangleBuffer->getVertices();

    for (int i = 0; i < vertices->size(); i += 3) {

        Triangle* tri = new Triangle((*vertices)[i], (*vertices)[i + 1], (*vertices)[i + 2]);
        //if geometry data is contained within the first octet then build it out
        if (GeometryMath::triangleCubeDetection(tri, rootCube)) {

            node->addGeometry(entity, tri, i);
        }
    }

    //Recursively build Octary Space Partition Tree
    _buildOctetTree(_octTree.getRoot()->getData(), node);
}

std::vector<Cube>* OSP::getFrustumCubes() {
    std::vector<Cube>* cubes = new std::vector<Cube>();
    //Look through all 8 subspaces in the octree
    for (auto leaf : _frustumLeaves) {
        cubes->push_back(*leaf->getData());
    }
    return cubes;
}

std::vector<Cube>* OSP::getCubes() {

    std::vector<Cube>* cubes = new std::vector<Cube>();
    
    _getChildren(cubes, _octTree.getRoot());

    return cubes;
}

void OSP::_getChildren(std::vector<Cube>* cubes, OctNode<Cube*>* node) {
    
    //Grab all of the possible subspace children in the tree
    auto childrenNodes = node->getChildren();

    //Look through all 8 subspaces in the octree
    for (int i = 0; i < 8; ++i) {
        if (childrenNodes[i] != nullptr) {
            _getChildren(cubes, childrenNodes[i]);
            cubes->push_back(*childrenNodes[i]->getData());
        }
    }
}

std::vector<int> OSP::getVisibleFrustumCulling() {

    _frustumLeaves.clear();

    Matrix inverseViewProjection = ModelBroker::getViewManager()->getFrustumView().inverse() * 
        ModelBroker::getViewManager()->getFrustumProjection().inverse();

    std::vector<Vector4> frustumPlanes;
    GeometryMath::getFrustumPlanes(inverseViewProjection, frustumPlanes);

    int i = 1; //offset from master vbo
    std::vector<int> vbosToDraw;
    std::vector<std::pair<int, float>> vaoDepths;
    for (OctNode<Cube*>* octNode : _ospLeaves) {

        if (octNode->getTriangles()->size() != 0) {

            auto cube = octNode->getData();
            auto center = cube->getCenter();
            auto length = cube->getLength();
            auto height = cube->getHeight();
            auto width = cube->getWidth();
            Vector4 mins(center.getx() - length / 2.0f, center.gety() - height / 2.0f, center.getz() - width / 2.0f);
            Vector4 maxs(center.getx() + length / 2.0f, center.gety() + height / 2.0f, center.getz() + width / 2.0f);

            if (GeometryMath::frustumAABBDetection(frustumPlanes, mins, maxs)) {
                _frustumLeaves.push_back(octNode);
                vaoDepths.push_back(std::pair<int, float>(i, center.getMagnitude()));
            }
        }
        i++;
    }

    // sort using a custom function object
    struct {
        bool operator()(std::pair<int, float> a, std::pair<int, float> b) const
        {
            return a.second < b.second;
        }
    } customLess;

    std::sort(vaoDepths.begin(), vaoDepths.end(), customLess);

    //Depth order vbos front to back for early vertex discard
    for (auto vaoDepth : vaoDepths) {
        vbosToDraw.push_back(vaoDepth.first);
    }

    return vbosToDraw;
}

void OSP::updateOSP(std::vector<Entity*>& entities) {

    //Go through all of the models and populate
    for (auto entity : entities){
        if (entity->getStateVector()->getActive()) { //Only do osp updates if the model is active

            std::vector<Sphere>* spheres = entity->getGeometry()->getSpheres();
            for (Sphere& sphere : *spheres) {

                ////Early out test if the sphere is completely contained within this cube,
                ////otherwise if the sphere is somewhat outside (protrudes) of it's native cube then update OSP
                //std::set<Cube*>& cubes = _sphereCubeCache[&sphere];
                //for (Cube* cube : cubes) {
                //    if (GeometryMath::sphereProtrudesCube(&sphere, cube)) {
                //        //_sphereCubeCache[&sphere].clear(); //Need to create a new sphere cache from scratch
                //        _insertSphereSubspaces(model, sphere, _octTree.getRoot());
                //    }
                //}
                
                _sphereCubeCache[&sphere].clear();
                _insertSphereSubspaces(entity, sphere, _octTree.getRoot());
            }
        }
    }

}

bool OSP::_insertSphereSubspaces(Entity* entity, Sphere& sphere, OctNode<Cube*>* node) {

    //Used to only insert a new cache cube location when the end of the octree is reached
    bool inserted = false;

    //Grab all of the possible subspace children in the tree
    auto childrenNodes = node->getChildren();

    //Look through all 8 subspaces in the octree
    for (int i = 0; i < 8; ++i) {

        //If the sphere is in this cube then keep looking in the subdivision tree
        if (childrenNodes[i] != nullptr && GeometryMath::sphereCubeDetection(&sphere, childrenNodes[i]->getData())) {

            //Recursively call until end of tree is found
            if (!_insertSphereSubspaces(entity, sphere, childrenNodes[i])) {
                _sphereCubeCache[&sphere].insert(childrenNodes[i]->getData());
                inserted = true;
            }
            //Add geometry to model
            childrenNodes[i]->addGeometry(entity, &sphere);
        }
    }
    return inserted;
}


void OSP::_buildOctetTree(Cube* cube, OctNode<Cube*>* node) {

    //Oct tree will be split up into 8 equal spaced 3D cubes every time the primitive count in a cube has exceeded
    //the maxGeometries parameter

    float cubicDimension = cube->getLength() / 2.0f;// Take any dimension and divide by 2
    float dim = cubicDimension / 2.0f; //New cubic position values
    Vector4 pos = cube->getCenter();

    std::vector<Cube*> cubes;
    cubes.push_back(new Cube(cubicDimension, cubicDimension, cubicDimension, Vector4(-dim, -dim, -dim, 1) + pos));
    cubes.push_back(new Cube(cubicDimension, cubicDimension, cubicDimension, Vector4(-dim, -dim, dim, 1) + pos));
    cubes.push_back(new Cube(cubicDimension, cubicDimension, cubicDimension, Vector4(-dim, dim, dim, 1) + pos));
    cubes.push_back(new Cube(cubicDimension, cubicDimension, cubicDimension, Vector4(dim, -dim, -dim, 1) + pos));
    cubes.push_back(new Cube(cubicDimension, cubicDimension, cubicDimension, Vector4(dim, -dim, dim, 1) + pos));
    cubes.push_back(new Cube(cubicDimension, cubicDimension, cubicDimension, Vector4(dim, dim, -dim, 1) + pos));
    cubes.push_back(new Cube(cubicDimension, cubicDimension, cubicDimension, Vector4(-dim, dim, -dim, 1) + pos));
    cubes.push_back(new Cube(cubicDimension, cubicDimension, cubicDimension, Vector4(dim, dim, dim, 1) + pos));

    std::vector<OctNode<Cube*>*> nodes;
    for (auto cube : cubes) {
        nodes.push_back(_octTree.insert(node, cube));
    }
    std::vector<int> cubesEntered(8, 0);

    auto triangleMaps = node->getTriangles();

    
    for (std::pair<Entity* const, std::set<std::pair<int, Triangle*>>>& triangleMap : *node->getTriangles()) {
        for (auto triangleIndex : triangleMap.second) {

            int cubeIndex = 0;
            for (auto cube : cubes) {

                //if geometry data is contained within the first octet then build it out
                if (GeometryMath::triangleCubeDetection(triangleIndex.second, cube)) {
                    nodes[cubeIndex]->addGeometry(triangleMap.first, triangleIndex.second, triangleIndex.first);
                    cubesEntered[cubeIndex]++;
                }
                cubeIndex++;
            }
        }
    }

    std::unordered_map<Entity*, std::set<Sphere*>>* sphereMaps = node->getSpheres();
    for (std::pair<Entity* const, std::set<Sphere*>>& sphereMap : *sphereMaps) {
        for (Sphere* sphere : sphereMap.second) {

            int cubeIndex = 0;
            for (auto cube : cubes) {

                //if geometry data is contained within the first octet then build it out
                if (GeometryMath::sphereCubeDetection(sphere, cube)) {

                    nodes[cubeIndex]->addGeometry(sphereMap.first, sphere);
                    cubesEntered[cubeIndex]++;
                }
                cubeIndex++;
            }
        }
    }

    int cubeIndex = 0;
    for (auto cube : cubes) {
        if (cubesEntered[cubeIndex] > _maxGeometries) {
            _buildOctetTree(cube, nodes[cubeIndex]); //Recursive call to dig deeper into octary space partition tree
        }
        //If a subspace has less than _maxGeometries primitive count then add it to the leaves list for collision detection
        else {
            _ospLeaves.push_back(nodes[cubeIndex]);

            //Recheck spheres location to cache their locations
            std::unordered_map<Entity*, std::set<Sphere*>>* sphereMaps = node->getSpheres();
            for (std::pair<Entity* const, std::set<Sphere*>>& sphereMap : *sphereMaps) {
                for (Sphere* sphere : sphereMap.second) {
                    //if geometry data is contained within the first octet then build it out
                    if (GeometryMath::sphereCubeDetection(sphere, cube)) {
                        //Cache the location of each sphere pointer to a set of cubes
                        _sphereCubeCache[sphere].insert(cube);
                    }
                }
            }
        }
        cubeIndex++;
    }
}
