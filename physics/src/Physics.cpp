#include "Physics.h"
#include "GeometryMath.h"

Physics::Physics() {
   
}

Physics::~Physics() {

}

void Physics::run() {

    MasterClock* clock = MasterClock::instance();
    clock->subscribeKinematicsRate(std::bind(&Physics::_physicsProcess, this, std::placeholders::_1));
}

void Physics::addModels(std::vector<Model*> models) {
    _models.insert(_models.end(), models.begin(), models.end());
}

void Physics::addModel(Model* model) {
    _models.push_back(model);
}

void Physics::_physicsProcess(int milliseconds) {

    int modelPosition = 1; //Used to prevent redudant collision tests
    for(auto model : _models) {
        
        for(int modelIndex = modelPosition; modelIndex < _models.size(); ++modelIndex) {

            if(model->getGeometryType() == GeometryType::Triangle && _models[modelIndex]->getGeometryType() == GeometryType::Sphere) {
                GeometryMath::sphereTriangleDetection(_models[modelIndex], model);
            }
            else if(model->getGeometryType() == GeometryType::Sphere && _models[modelIndex]->getGeometryType() == GeometryType::Triangle) {
                GeometryMath::sphereTriangleDetection(model, _models[modelIndex]);
            } 
            else if(model->getGeometryType() == GeometryType::Sphere && _models[modelIndex]->getGeometryType() == GeometryType::Sphere) {
                GeometryMath::sphereSphereDetection(model, _models[modelIndex]);
            }

        }
        ++modelPosition;
    }
}