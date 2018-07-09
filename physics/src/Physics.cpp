#include "Physics.h"
#include "GeometryMath.h"
#include "ShaderBroker.h"

//Make OSP (Octal Space Partioner) a 2000 cubic block and ensure only 500 primitives at maximum
//are within a subspace of the OSP
Physics::Physics() : 
    _octalSpacePartioner(2000, 500),
    _debugShader(static_cast<DebugShader*>(ShaderBroker::instance()->getShader("debugShader"))) {

}

Physics::~Physics() {

}

void Physics::run() {

    MasterClock* clock = MasterClock::instance();
    clock->subscribeKinematicsRate(std::bind(&Physics::_physicsProcess, this, std::placeholders::_1));
}

void Physics::addModels(std::vector<Model*> models) {

    _models.insert(_models.end(), models.begin(), models.end());

    for (auto model : _models) {
        GeometryType geomType = model->getGeometryType();
       
        if (geomType == GeometryType::Sphere) {
            _graphics.push_back(new GeometryGraphic(model->getGeometry()->getSpheres()));
        }
        else if (geomType == GeometryType::Triangle) {
            _graphics.push_back(new GeometryGraphic(model->getGeometry()->getTriangles()));
        }
    }

    _octalSpacePartioner.generateGeometryOSP(_models); //Generate the octal space partition for collision efficiency

    _octTreeGraphic = new GeometryGraphic(_octalSpacePartioner.getCubes());
}

void Physics::addModel(Model* model) {
    _models.push_back(model);
}

void Physics::visualize() {
    
    
    int i = 0;
    for (auto geometryGraphic : _graphics) {

        float color[] = { 0.0, 0.0, 1.0 };

        _debugShader->runShader(_models[i]->getMVP(), 
            geometryGraphic->getVAO(), 
            _triangleIntersectionList[_models[i]],
            color);
        i++;
    }

    float color[] = { 0.0, 1.0, 0.0 };
    //Grab any model's mvp
    _debugShader->runShader(_models[0]->getMVP(), 
        _octTreeGraphic->getVAO(), 
        _triangleIntersectionList[_models[0]],
        color);

    std::lock_guard<std::mutex> lockGuard(_lock);

    _triangleIntersectionList.clear();
}

void Physics::_physicsProcess(int milliseconds) {

    //First update OSP tree then test for collisions
    _octalSpacePartioner.updateOSP(_models);

    //Returns the subspace partitioning node leaves to test for primitive collisions
    //The nodes necessary to test for collisions are only the end nodes of the oct tree

    std::vector<bool> prevContactStates;
    std::map<Model*, bool> newContactStates;
    for (Model* model : _models) {
        prevContactStates.push_back(model->getStateVector()->getContact());
    }

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

                                    //GeometryMath::sphereSphereResolution(sphereMapA.first, *sphereA, sphereMapB.first, *sphereB);
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

                        if (GeometryMath::sphereCubeDetection(sphere, subspaceNode->getData())) {

                            bool prevContactState = modelSphereState->getContact();
                            bool newContactState = false;

                            std::lock_guard<std::mutex> lockGuard(_lock);

                            int i = 0;
                            for (Triangle* triangle : triangles) {
                                //If an overlap between a sphere and a triangle is detected then process the overlap resolution
                                if (GeometryMath::sphereTriangleDetection(*sphere, *triangle)) {

                                    GeometryMath::sphereTriangleResolution(sphereMap.first, *sphere, triangleMap.first, *triangle);
                                    newContactState = true;
                                    _triangleIntersectionList[sphereMap.first].insert(triangle);
                                }
                                i++;
                            }

                            newContactStates[sphereMap.first] = newContactState;
                            ////If there was a previous contact and now there is no contact then set contact to false
                            //if(prevContactState && !newContactState) {
                            //    modelSphereState->setContact(false);
                            //}
                        }
                        else {
                            //TODO: Fix me because this was causing a crash most likely
                            // from removing a geometry from the oct node and iterating at the same time.
                            //Remove geometry from osp node
                            //subspaceNode->removeGeometry(sphereMap.first, sphere);
                        }
                    }
                }
            }
        }

        //Triangle on triangle detections...probably will NOT implement...maybe some day

    }

    int i = 0;
    for (Model* model : _models) {
        if (prevContactStates[i++] && !newContactStates[model]) {
            model->getStateVector()->setContact(false);
        }
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