#include "FrustumCuller.h"
#include "Model.h"
#include "ShaderBroker.h"
#include "ModelBroker.h"
#include "ViewManager.h"

FrustumCuller::FrustumCuller(std::vector<Entity*> entities) :
    _octalSpacePartitioner(2000, 4000),
    _debugShader(static_cast<DebugShader*>(ShaderBroker::instance()->getShader("debugShader"))) {
      
    _octalSpacePartitioner.generateRenderOSP(entities); //Generate 

    auto frustumAABBs = _octalSpacePartitioner.getFrustumLeaves();

    entities[0]->getModel()->generateVAOTiles(this);

    _octTreeGraphic = new GeometryGraphic(_octalSpacePartitioner.getFrustumCubes());
}

OSP* FrustumCuller::getOSP() {
    return &_octalSpacePartitioner;
}

void FrustumCuller::visualize() {

    float color[] = { 0.0, 1.0, 0.0 };
    //Grab any model's mvp
    MVP mvp;
    mvp.setView(ModelBroker::getViewManager()->getView());
    mvp.setProjection(ModelBroker::getViewManager()->getProjection());
    _debugShader->runShader(&mvp,
        _octTreeGraphic->getVAO(),
        {},
        color,
        GeometryConstruction::LINE_WIREFRAME);
}

std::vector<int> FrustumCuller::getVisibleVBOs() {
    _octTreeGraphic = new GeometryGraphic(_octalSpacePartitioner.getFrustumCubes());
    return _octalSpacePartitioner.getVisibleFrustumCulling();
}