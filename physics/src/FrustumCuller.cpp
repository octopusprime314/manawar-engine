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

bool FrustumCuller::getVisibleAABB(Entity* entity, Matrix inverseViewProjection){

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

        //std::vector<Cube>* cubes = new std::vector<Cube>{
        //    Cube(length, height, width, center) };
        //VAO vao;
        //vao.createVAO(cubes, GeometryConstruction::LINE_WIREFRAME);

        ////Build inverse projection matrix which will properly transform unit cube
        ////to the frustum vertices to visualize it in the engine
        //MVP mvp;

        ////Model transform to create frustum cube
        //Matrix cameraView = ModelBroker::getViewManager()->getView();
        //Matrix cameraProj = ModelBroker::getViewManager()->getProjection();
        //mvp.setView(cameraView);
        //mvp.setProjection(cameraProj);

        //auto debugShader = static_cast<DebugShader*>(ShaderBroker::instance()->getShader("debugShader"));
        //Vector4 color(1.0, 0.0, 0.0);
        //debugShader->runShader(&mvp, &vao, {}, color.getFlatBuffer(), GeometryConstruction::LINE_WIREFRAME);
        return true;
    }
    else {
        return false;
    }
}

bool FrustumCuller::getVisibleOBB(Entity* entity, Matrix inverseViewProjection, Light* light) {

    Matrix  lightView  = light->getLightMVP().getViewMatrix();
    Cube*   entityAABB = entity->getModel()->getGfxAABB();
    float*  buffer     = entity->getWorldSpaceTransform().getFlatBuffer();
    Vector4 aRot       = Vector4(buffer[0], buffer[1], buffer[2]);
    Vector4 bRot       = Vector4(buffer[4], buffer[5], buffer[6]);
    Vector4 cRot       = Vector4(buffer[8], buffer[9], buffer[10]);
    auto    length     = entityAABB->getLength() * aRot.getMagnitude();
    auto    height     = entityAABB->getHeight() * bRot.getMagnitude();
    auto    width      = entityAABB->getWidth()  * cRot.getMagnitude();
    auto    pos        = entity->getStateVector()->getLinearPosition().getFlatBuffer();
    Matrix translation = Matrix::translation(-buffer[3], buffer[7], buffer[11]) *
                         Matrix::translation(pos[0], pos[1], pos[2]);
    float  planeY      = fabs(light->getRange()) / /*8.0f*/2.0f;

    lightView.getFlatBuffer()[3]  = 0.0f;
    lightView.getFlatBuffer()[7]  = 0.0f;
    lightView.getFlatBuffer()[11] = 0.0f;

    std::vector<Vector4> points;
    points.push_back(Vector4(-(length / 2.0f), -(height / 2.0f) - planeY, -(width / 2.0f)));
    points.push_back(Vector4(+(length / 2.0f), +(height / 2.0f) + planeY, -(width / 2.0f)));
    points.push_back(Vector4(+(length / 2.0f), -(height / 2.0f) - planeY, -(width / 2.0f)));
    points.push_back(Vector4(-(length / 2.0f), +(height / 2.0f) + planeY, -(width / 2.0f)));
    points.push_back(Vector4(+(length / 2.0f), -(height / 2.0f) - planeY, +(width / 2.0f)));
    points.push_back(Vector4(-(length / 2.0f), +(height / 2.0f) + planeY, +(width / 2.0f)));
    points.push_back(Vector4(-(length / 2.0f), -(height / 2.0f) - planeY, +(width / 2.0f)));
    points.push_back(Vector4(+(length / 2.0f), +(height / 2.0f) + planeY, +(width / 2.0f)));


    float min[3] = { (std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)() };
    float max[3] = { (std::numeric_limits<float>::min)(), (std::numeric_limits<float>::min)(), (std::numeric_limits<float>::min)() };

    for (int i = 0; i < points.size(); i++) {
        float* a = points[i].getFlatBuffer();

        for (int j = 0; j < 3; j++) {
            if (a[j] < min[j]) {
                min[j] = a[j];
            }
            if (a[j] > max[j]) {
                max[j] = a[j];
            }
        }
    }
    
    //Make seperation at least 0.1
    if (max[0] - min[0] < 0.1) {
        max[0] = min[0] + 0.1f;
    }
    if (max[1] - min[1] < 0.1) {
        max[1] = min[1] + 0.1f;
    }
    if (max[2] - min[2] < 0.1) {
        max[2] = min[2] + 0.1f;
    }

    Vector4 mins(min[0], min[1], min[2]);
    Vector4 maxs(max[0], max[1], max[2]);

    Matrix lightInverseViewProjection = lightView.inverse() * translation/*.inverse()*/ * inverseViewProjection;

    std::vector<Vector4> frustumPlanes;
    GeometryMath::getFrustumPlanes(lightInverseViewProjection, frustumPlanes);

    if (GeometryMath::frustumAABBDetection(frustumPlanes, mins, maxs)) {

        ////Transformed
        //static std::vector<Cube>* hitOBB = new std::vector<Cube>{
        //    Cube(max[0] - min[0], max[1] - min[1], max[2] - min[2], Vector4(0.0,0.0,0.0)) };

        //static VAO hitVAO;
        //hitVAO.createVAO(hitOBB, GeometryConstruction::LINE_WIREFRAME);

        ////Build inverse projection matrix which will properly transform unit cube
        ////to the frustum vertices to visualize it in the engine
        //MVP hitMVP;

        ////Model transform to create frustum cube
        //Matrix cameraView = ModelBroker::getViewManager()->getView();
        //Matrix cameraProj = ModelBroker::getViewManager()->getProjection();
        //Matrix visTranslation = Matrix::translation(buffer[3], buffer[7], -buffer[11]) *
        //    Matrix::translation(pos[0], pos[1], pos[2]);
        //hitMVP.setView(cameraView * visTranslation * lightView.inverse());
        //hitMVP.setProjection(cameraProj);

        //auto debugShader = static_cast<DebugShader*>(ShaderBroker::instance()->getShader("debugShader"));
        //Vector4 color(1.0, 0.0, 0.0);
        //debugShader->runShader(&hitMVP, &hitVAO, {}, color.getFlatBuffer(), GeometryConstruction::LINE_WIREFRAME);

        return true;
    }
    else {
        return false;
    }
}