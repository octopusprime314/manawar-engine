#include "OSP.h"
#include "GeometryMath.h"
#include "ModelBroker.h"
#include "ViewManager.h"

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
        std::vector<Triangle>* triangles = entity->getModel()->getGeometry()->getTriangles();

        for (Triangle & triangle : *triangles) {
            //if geometry data is contained within the first octet then build it out
            if (GeometryMath::triangleCubeDetection(&triangle, rootCube)) {

                node->addGeometry(entity, &triangle);
            }
        }

        std::vector<Sphere>* spheres = entity->getModel()->getGeometry()->getSpheres();
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

void OSP::generateRenderOSP(std::vector<Entity*>& entities) {


    //Initialize a octary tree with a rectangle of cubicDimension located at the origin of the axis
    Cube* rootCube = new Cube(_cubicDimension, _cubicDimension, _cubicDimension, Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    OctNode<Cube*>* node = _octTree.insert(nullptr, rootCube);

    //Go through all of the models and populate
    for (auto entity : entities) {

        auto triangleBuffer = entity->getModel()->getRenderBuffers();
        auto vertices = triangleBuffer->getVertices();

        for (int i = 0; i < vertices->size(); i+=3) {
             
            Triangle* tri = new Triangle((*vertices)[i], (*vertices)[i + 1], (*vertices)[i + 2]);
            //if geometry data is contained within the first octet then build it out
            if (GeometryMath::triangleCubeDetection(tri, rootCube)) {

                node->addGeometry(entity, tri, i);
            }
        }
    }

    //Recursively build Octary Space Partition Tree
    _buildOctetTree(_octTree.getRoot()->getData(), node);

    //TODO : Take one models mvp which is easy but needs to be changed soon 
    /*MVP* mvpBuffers = entities[0]->getMVP();
    float* mvp = (mvpBuffers->getProjectionMatrix()).inverse().getFlatBuffer();*/

    float* mvp = ModelBroker::getViewManager()->getProjection().inverse().getFlatBuffer();

    //float* mvp = (mvpBuffers->getViewMatrix() * 
    //             (mvpBuffers->getProjectionMatrix()).inverse()).getFlatBuffer();

    std::vector<Cube*> visibleCubes;

    for (OctNode<Cube*>* octNode : _ospLeaves) {
        
        auto cube   = octNode->getData();
        auto center = cube->getCenter();
        auto length = cube->getLength();
        auto height = cube->getHeight();
        auto width  = cube->getWidth();
        Vector4 mins(center.getx() - length / 2.0f, center.gety() - height / 2.0f, center.getz() - width / 2.0f);
        Vector4 maxs(center.getx() + length / 2.0f, center.gety() + height / 2.0f, center.getz() + width / 2.0f);

        std::vector<Vector4> planes;
        // Right clipping plane.
        planes.push_back(Vector4(mvp[3] - mvp[0], mvp[7] - mvp[4], mvp[11] - mvp[8],  mvp[15] - mvp[12]));
        // Left clipping plane.
        planes.push_back(Vector4(mvp[3] + mvp[0], mvp[7] + mvp[4], mvp[11] + mvp[8],  mvp[15] + mvp[12]));
        // Bottom clipping plane.
        planes.push_back(Vector4(mvp[3] + mvp[1], mvp[7] + mvp[5], mvp[11] + mvp[9],  mvp[15] + mvp[13]));
        // Top clipping plane.
        planes.push_back(Vector4(mvp[3] - mvp[1], mvp[7] - mvp[5], mvp[11] - mvp[9],  mvp[15] - mvp[13]));
        // Far clipping plane.
        planes.push_back(Vector4(mvp[3] - mvp[2], mvp[7] - mvp[6], mvp[11] - mvp[10], mvp[15] - mvp[14]));
        // Near clipping plane.
        planes.push_back(Vector4(mvp[3] + mvp[2], mvp[7] + mvp[6], mvp[11] + mvp[10], mvp[15] + mvp[14]));

        //// Right clipping plane.
        //planes.push_back(Vector4(mvp[12] - mvp[0], mvp[13] - mvp[1], mvp[14] - mvp[2], mvp[15] - mvp[3]));
        //// Left clipping plane.
        //planes.push_back(Vector4(mvp[12] + mvp[0], mvp[13] + mvp[1], mvp[14] + mvp[2], mvp[15] + mvp[3]));
        //// Bottom clipping plane.
        //planes.push_back(Vector4(mvp[12] + mvp[4], mvp[13] + mvp[5], mvp[14] + mvp[6], mvp[15] + mvp[7]));
        //// Top clipping plane.
        //planes.push_back(Vector4(mvp[12] - mvp[4], mvp[13] - mvp[5], mvp[14] - mvp[6], mvp[15] - mvp[7]));
        //// Far clipping plane.
        //planes.push_back(Vector4(mvp[12] - mvp[8], mvp[13] - mvp[9], mvp[14] - mvp[10], mvp[15] - mvp[11]));
        //// Near clipping plane.
        //planes.push_back(Vector4(mvp[12] + mvp[8], mvp[13] + mvp[9], mvp[14] + mvp[10], mvp[15] + mvp[11]));

        // Normalize, this is not always necessary...
        /*for (unsigned int i = 0; i < 6; i++) {
            planes[i].normalize();
        }*/
        
        if (GeometryMath::frustumAABBDetection(planes, mins, maxs)) {
            _frustumLeaves.push_back(octNode);
        }
    }
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


void OSP::updateOSP(std::vector<Entity*>& entities) {

    //Go through all of the models and populate
    for (auto entity : entities){
        if (entity->getStateVector()->getActive()) { //Only do osp updates if the model is active

            std::vector<Sphere>* spheres = entity->getModel()->getGeometry()->getSpheres();
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

    std::unordered_map<Entity*, std::set<Triangle*>>* triangleMaps = node->getTriangles();
    auto triangleIndices = node->getTriangleIndices();
    int i = 0;
    for (std::pair<Entity* const, std::set<Triangle*>>& triangleMap : *triangleMaps) {
        for (Triangle* triangle : triangleMap.second) {

            int cubeIndex = 0;
            for (auto cube : cubes) {

                //if geometry data is contained within the first octet then build it out
                if (GeometryMath::triangleCubeDetection(triangle, cube)) {

                    if (_frustumLeaves.size() > 0) {
                        nodes[cubeIndex]->addGeometry(triangleMap.first, triangle, triangleIndices[i]);
                        cubesEntered[cubeIndex]++;
                    }
                    else {
                        nodes[cubeIndex]->addGeometry(triangleMap.first, triangle);
                        cubesEntered[cubeIndex]++;
                    }
                }
                cubeIndex++;
            }
        }
        i++;
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
