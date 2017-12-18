#include "Physics.h"
#include "GeometryMath.h"

//Make OSP (Octal Space Partioner) a 2000 cubic block and ensure only 500 primitives at maximum
//are within a subspace of the OSP
Physics::Physics() : _octalSpacePartioner(2000, 500) {

}

Physics::~Physics() {

}

void Physics::run() {

    MasterClock* clock = MasterClock::instance();
    clock->subscribeKinematicsRate(std::bind(&Physics::_physicsProcess, this, std::placeholders::_1));
}

void Physics::addModels(std::vector<Model*> models) {

    _models.insert(_models.end(), models.begin(), models.end());

    _octalSpacePartioner.generateOSP(_models); //Generate the octal space partition for collision efficiency
}

void Physics::addModel(Model* model) {
    _models.push_back(model);
}

void Physics::_physicsProcess(int milliseconds) {

    //First update OSP tree then test for collisions
    _octalSpacePartioner.updateOSP(_models);


    //Returns the subspace partitioning node leaves to test for primitive collisions
    //The nodes necessary to test for collisions are only the end nodes of the oct tree

    auto ospEndNodes = _octalSpacePartioner.getOSPLeaves();

    for (OctNode<Cube*> * subspaceNode : *ospEndNodes) {

        auto sphereMaps = subspaceNode->getSpheres();
        auto triangleMaps = subspaceNode->getTriangles();

        //Sphere on sphere detections
        for (std::pair<Model* const, std::set<Sphere*>>& sphereMapA : *sphereMaps) {

            StateVector* modelSphereStateA = sphereMapA.first->getStateVector();

            for (std::pair<Model* const, std::set<Sphere*>>& sphereMapB : *sphereMaps) {

                //Only do detections for different models, do not detect an overlap for a model on itself...
                if (sphereMapA.first != sphereMapB.first) {

                    StateVector* modelSphereStateB = sphereMapB.first->getStateVector();
                    if (modelSphereStateA->getActive() || modelSphereStateB->getActive()) { //Only test for collisions if one of the models is active

                        std::set<Sphere*>& spheresA = sphereMapA.second;
                        std::set<Sphere*>& spheresB = sphereMapB.second;

                        for (Sphere* sphereA : spheresA) {
                            for (Sphere* sphereB : spheresB) {
                                //If an overlap between a sphere and a sphere is detected then process the overlap resolution
                                if (GeometryMath::sphereSphereDetection(*sphereA, *sphereB)) {

                                    GeometryMath::sphereSphereResolution(sphereMapA.first, *sphereA, sphereMapB.first, *sphereB);
                                }
                            }
                        }
                    }
                }
            }
        }

        //Sphere on triangle detections
        for (std::pair<Model* const, std::set<Sphere*>>& sphereMap : *sphereMaps) {

            StateVector* modelSphereState = sphereMap.first->getStateVector();

            for (std::pair<Model* const, std::set<Triangle*>>& triangleMap : *triangleMaps) {

                StateVector* modelTriangleState = triangleMap.first->getStateVector();
                if (modelSphereState->getActive() || modelTriangleState->getActive()) { //Only test for collisions if one of the models is active

                    std::set<Triangle*>& triangles = triangleMap.second;
                    std::set<Sphere*>& spheres = sphereMap.second;

                    for (Sphere* sphere : spheres) {
                        for (Triangle* triangle : triangles) {
                            //If an overlap between a sphere and a triangle is detected then process the overlap resolution
                            if (GeometryMath::sphereTriangleDetection(*sphere, *triangle)) {

                                GeometryMath::sphereTriangleResolution(sphereMap.first, *sphere, triangleMap.first, *triangle);
                            }
                        }
                    }
                }
            }
        }

        //Triangle on triangle detections...probably will NOT implement...maybe some day

    }
}

void Physics::_slowDetection() {

    // Slow collision detection that does not involve space partitioning
    int modelPosition = 1; //Used to prevent redudant collision tests
    for (auto model : _models) {

        for (int modelIndex = modelPosition; modelIndex < _models.size(); ++modelIndex) {
            GeometryType geomTypeA = model->getGeometryType();
            GeometryType geomTypeB = _models[modelIndex]->getGeometryType();
            if (geomTypeA == GeometryType::Triangle && geomTypeB == GeometryType::Sphere) {
                GeometryMath::spheresTrianglesDetection(_models[modelIndex], model);
            }
            else if (geomTypeA == GeometryType::Sphere && geomTypeB == GeometryType::Triangle) {
                GeometryMath::spheresTrianglesDetection(model, _models[modelIndex]);
            }
            else if (geomTypeA == GeometryType::Sphere && geomTypeB == GeometryType::Sphere) {
                GeometryMath::spheresSpheresDetection(model, _models[modelIndex]);
            }

        }
        ++modelPosition;
    }
}


