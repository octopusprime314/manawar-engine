#include "FrustumCuller.h"
#include "Model.h"
#include "ShaderBroker.h"
#include "ModelBroker.h"
#include "ViewEventDistributor.h"
#include "GeometryMath.h"

FrustumCuller::FrustumCuller(Entity* entity, float aabbDimension, int aabbMaxTriangles) :
    _octalSpacePartitioner(new OSP(aabbDimension, aabbMaxTriangles)),
    _debugShader(static_cast<DebugShader*>(ShaderBroker::instance()->getShader("debugShader"))) {
      
    _octalSpacePartitioner->generateRenderOSP(entity); //Generate 

    auto frustumAABBs = _octalSpacePartitioner->getFrustumLeaves();
    _octTreeGraphic = new GeometryGraphic(_octalSpacePartitioner->getFrustumCubes());
}

FrustumCuller::FrustumCuller(Entity* entity, Cube aabbCube) :
    _octalSpacePartitioner(nullptr),
    _debugShader(static_cast<DebugShader*>(ShaderBroker::instance()->getShader("debugShader"))) {

    _singleAABB = { aabbCube };
    _octTreeGraphic = new GeometryGraphic(&_singleAABB);
}

OSP* FrustumCuller::getOSP() {
    return _octalSpacePartitioner;
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

std::vector<int> FrustumCuller::getVisibleVAOs() {
    
    _octTreeGraphic = new GeometryGraphic(_octalSpacePartitioner->getFrustumCubes());
    return _octalSpacePartitioner->getVisibleFrustumCulling();
}

bool FrustumCuller::getVisibleVAO(Entity* entity) {
    
    Matrix inverseViewProjection = ModelBroker::getViewManager()->getFrustumView().inverse() *
            ModelBroker::getViewManager()->getFrustumProjection().inverse();

    std::vector<Vector4> frustumPlanes;
    GeometryMath::getFrustumPlanes(inverseViewProjection, frustumPlanes);

    auto pos = entity->getStateVector()->getLinearPosition();
    auto center = Matrix::translation(pos.getx(), pos.gety(), pos.getz()) * 
        _singleAABB[0].getCenter();
    auto length = _singleAABB[0].getLength();
    auto height = _singleAABB[0].getHeight();
    auto width = _singleAABB[0].getWidth();
    Vector4 mins(center.getx() - length / 2.0f, center.gety() - height / 2.0f, center.getz() - width / 2.0f);
    Vector4 maxs(center.getx() + length / 2.0f, center.gety() + height / 2.0f, center.getz() + width / 2.0f);

    std::vector<Cube> cubes = { Cube(length, height, width, center) };
    _octTreeGraphic = new GeometryGraphic(&cubes);

    if (GeometryMath::frustumAABBDetection(frustumPlanes, mins, maxs)) {
        return true;
    }
    else {
        return false;
    }
}

bool FrustumCuller::getVisible(Entity* entity, Matrix inverseViewProjection){

    std::vector<Vector4> frustumPlanes;
    GeometryMath::getFrustumPlanes(inverseViewProjection, frustumPlanes);

    Cube*   entityAABB  = entity->getModel()->getGfxAABB();

    Matrix trans   = entity->getWorldSpaceTransform();
    Vector4 center = trans * (entityAABB->getCenter() + entity->getStateVector()->getLinearPosition());
    
    float* buffer = entity->getWorldSpaceTransform().getFlatBuffer();

    Vector4 A = Vector4(buffer[0], buffer[1], buffer[2]);
    Vector4 B = Vector4(buffer[4], buffer[5], buffer[6]);
    Vector4 C = Vector4(buffer[8], buffer[9], buffer[10]);

    auto length = entityAABB->getLength() * A.getMagnitude();
    auto height = entityAABB->getHeight() * B.getMagnitude();
    auto width  = entityAABB->getWidth()  * C.getMagnitude();

    Vector4 mins(center.getx() - length / 2.0f, center.gety() - height / 2.0f, center.getz() - width / 2.0f);
    Vector4 maxs(center.getx() + length / 2.0f, center.gety() + height / 2.0f, center.getz() + width / 2.0f);

    if (GeometryMath::frustumAABBDetection(frustumPlanes, mins, maxs)) {
        return true;
    }
    else {
        return false;
    }
}