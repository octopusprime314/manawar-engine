#include "FrustumCuller.h"
#include "Entity.h"

FrustumCuller::FrustumCuller(std::vector<Entity*> entities) :
    _octalSpacePartitioner(2000, 2000) {

    _octalSpacePartitioner.generateRenderOSP(entities); //Generate 

    auto frustumAABBs = _octalSpacePartitioner.getFrustumLeaves();
}

OSP* FrustumCuller::getOSP() {
    return &_octalSpacePartitioner;
}