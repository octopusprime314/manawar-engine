#include "OSP.h"
#include "GeometryMath.h"

OSP::OSP(int cubicDimension, int maxGeometries) :
    _cubicDimension(cubicDimension),
    _maxGeometries(maxGeometries) {

}

OSP::~OSP() {

}

std::vector<OctNode<Cube*>*>* OSP::getOSPLeaves() {
    return &_ospLeaves;
}

void OSP::generateOSP(std::vector<Model*>& models) {


    //Initialize a octary tree with a rectangle of cubicDimension located at the origin of the axis
    Cube* rootCube = new Cube(_cubicDimension, _cubicDimension, _cubicDimension, Vector4(0, 0, 0, 1));
    OctNode<Cube*>* node = _octTree.insert(nullptr, rootCube);

    //Go through all of the models and populate 
    for (auto model : models) {
        std::vector<Triangle>* triangles = model->getGeometry()->getTriangles();

        for (Triangle & triangle : *triangles) {
            //if geometry data is contained within the first octet then build it out
            if (GeometryMath::triangleCubeDetection(&triangle, rootCube)) {

                node->addGeometry(model, &triangle);
            }
        }

        std::vector<Sphere>* spheres = model->getGeometry()->getSpheres();
        for (Sphere & sphere : *spheres) {

            //if geometry data is contained within the first octet then build it out
            if (GeometryMath::sphereCubeDetection(&sphere, rootCube)) {

                node->addGeometry(model, &sphere);
            }

        }
    }

    //Recursively build Octary Space Partition Tree
    _buildOctetTree(_octTree.getRoot()->getData(), node);
}

void OSP::updateOSP(std::vector<Model*>& models){


    //Go through all of the models and populate 
    for (auto model : models) {
        if (model->getStateVector()->getActive()) { //Only do osp updates if the model is active
            
            std::vector<Sphere>* spheres = model->getGeometry()->getSpheres();
            for (Sphere& sphere : *spheres) {

                for (auto cube : _ospLeaves) {
                    //Grab spheres to confirm whether this sphere is in a subspace already or leaving
                    std::set<Sphere*>& sphereList = (*cube->getSpheres())[model];
                    //if sphere is in this cube
                    if (GeometryMath::sphereCubeDetection(&sphere, cube->getData())) {

                        auto it = std::find (sphereList.begin(), sphereList.end(), &sphere);
                        //If sphere is not is subspace already then jump out
                        if(it == sphereList.end()){
                            cube->addGeometry(model, &sphere);
                        }
                    }
                    //remove sphere from subspace
                    else{
                        auto it = std::find (sphereList.begin(), sphereList.end(), &sphere);
                        //If sphere is not is subspace already then jump out
                        if(it != sphereList.end()){
                            cube->removeGeometry(model, &sphere);
                        }
                    }
                }
            }
        }
    }

    //for (OctNode<Cube*> * subspaceNode : _ospLeaves) {

    //    auto sphereMaps = subspaceNode->getSpheres();
    //    auto triangleMaps = subspaceNode->getTriangles();

    //    //Update subspace location of spheres within OSP
    //    for (std::pair<Model* const, std::vector<Sphere*>>& sphereMap : *sphereMaps) {

    //        StateVector* modelSphereState = sphereMap.first->getStateVector();

    //        if (modelSphereState->getActive()) { //Only do osp updates if the model is active

    //            std::vector<Sphere*>& spheres = sphereMap.second;

    //            for (Sphere* sphere : spheres) {

    //                //If sphere has moved out of subspace then update with new subspace location
    //                if (!GeometryMath::sphereCubeDetection(sphere, subspaceNode->getData())) {
    //                    //Update osp!!!!!
    //                }
    //            }
    //        }
    //    }

    //    //Update subspace location of triangles within OSP
    //    for (std::pair<Model* const, std::vector<Triangle*>>& triangleMap : *triangleMaps) {

    //        StateVector* modelTriangleState = triangleMap.first->getStateVector();
    //        if ( modelTriangleState->getActive()) { //Only do osp updates if the model is active

    //            std::vector<Triangle*>& triangles = triangleMap.second;

    //            for (Triangle* triangle : triangles) {

    //                //If triangle has moved out of subspace then update with new subspace location
    //                if (!GeometryMath::triangleCubeDetection(triangle, subspaceNode->getData())) {
    //                    //Update osp!!!!
    //                }
    //            }
    //        }
    //    }
    //}
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

    std::unordered_map<Model*, std::set<Triangle*>>* triangleMaps = node->getTriangles();

    for (std::pair<Model* const, std::set<Triangle*>>& triangleMap : *triangleMaps) {
        for (Triangle* triangle : triangleMap.second) {

            int cubeIndex = 0;
            for (auto cube : cubes) {

                //if geometry data is contained within the first octet then build it out
                if (GeometryMath::triangleCubeDetection(triangle, cube)) {

                    nodes[cubeIndex]->addGeometry(triangleMap.first, triangle);
                    cubesEntered[cubeIndex]++;
                }
                cubeIndex++;
            }
        }
    }

    std::unordered_map<Model*, std::set<Sphere*>>* sphereMaps = node->getSpheres();
    for (std::pair<Model* const, std::set<Sphere*>>& sphereMap : *sphereMaps) {
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
        }
        cubeIndex++;
    }
}