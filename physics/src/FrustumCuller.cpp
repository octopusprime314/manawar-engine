#include "FrustumCuller.h"
#include "Entity.h"
#include "ShaderBroker.h"

FrustumCuller::FrustumCuller(std::vector<Entity*> entities) :
    _octalSpacePartitioner(2000, 2000),
    _debugShader(static_cast<DebugShader*>(ShaderBroker::instance()->getShader("debugShader"))) {
      
    _octalSpacePartitioner.generateRenderOSP(entities); //Generate 

    auto frustumAABBs = _octalSpacePartitioner.getFrustumLeaves();

    _octTreeGraphic = new GeometryGraphic(_octalSpacePartitioner.getFrustumCubes());
}

OSP* FrustumCuller::getOSP() {
    return &_octalSpacePartitioner;
}

void FrustumCuller::visualize() {

    float color[] = { 0.0, 1.0, 0.0 };
    //Grab any model's mvp
    _debugShader->runShader((_entityList)[0]->getMVP(),
        _octTreeGraphic->getVAO(),
        {},
        color,
        GeometryConstruction::LINE_WIREFRAME);
}

void FrustumCuller::setEntityList(std::vector<Entity*>& entityList) {
    _entityList = entityList;
}

std::vector<int> FrustumCuller::getVisibleVBOs() {
    _octTreeGraphic = new GeometryGraphic(_octalSpacePartitioner.getFrustumCubes());
    return _octalSpacePartitioner.getVisibleFrustumCulling();
}