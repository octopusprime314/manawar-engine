#include "FrustumOcclusion.h"

FrustumOcclusion::FrustumOcclusion(std::vector<Model*> models) :
    _octalSpacePartitioner(2000, 500) {

    _octalSpacePartitioner.generateRenderOSP(models); //Generate 
}
