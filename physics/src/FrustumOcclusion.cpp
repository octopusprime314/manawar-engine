#include "FrustumOcclusion.h"
#include "Entity.h"

FrustumOcclusion::FrustumOcclusion(std::vector<Entity*> entities) :
    _octalSpacePartitioner(2000, 500) {

    _octalSpacePartitioner.generateRenderOSP(entities); //Generate 
}
