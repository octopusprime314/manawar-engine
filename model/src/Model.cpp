#include "Model.h"
#include "SimpleContext.h"
#include "FbxLoader.h"
#include "GeometryBuilder.h"

TextureBroker* Model::_textureManager = TextureBroker::instance();

Model::Model() {

}

Model::Model(std::string name, ViewManagerEvents* eventWrapper, ModelClass classId) : UpdateInterface(eventWrapper),
    _fbxLoader(nullptr),
    _clock(MasterClock::instance()) {

        //Set class id
        _classId = classId;

        //disable debug mode
        _debugMode = false;

        //Load debug shader
        _debugShaderProgram = new DebugShader("debugShader");

        //Load in fbx object 
        _fbxLoader = new FbxLoader(name);
        //Populate model with fbx file data and recursivelty search with the root node of the scene
        _fbxLoader->loadModel(this, _fbxLoader->getScene()->GetRootNode());

        //GeometryBuilder::buildCube(this); //Add a generic cube centered at the origin

        //Create vbo contexts
        _vbo.createVBO(&_renderBuffers, _classId);

        //If class is generic model then deallocate fbx object,
        //otherwise let derived class clean up _fbxLoader object
        //because the derived class may need to access data from it still
        if (_classId == ModelClass::ModelType) {

			//Load default shader
			_shaderProgram = new StaticShader("staticShader");

            //If the object is a standard model then it is modeled with triangles
            _geometryType = GeometryType::Triangle;

            delete _fbxLoader;

            std::string modelName = _getModelName(name);
            std::string colliderName = "../models/meshes/";
            colliderName.append(modelName).append("/collider.fbx");
            //Load in geometry fbx object 
            FbxLoader geometryLoader(colliderName);
            //Populate model with fbx file data and recursivelty search with the root node of the scene
            geometryLoader.loadGeometry(this, geometryLoader.getScene()->GetRootNode());
        }
        else if (_classId == ModelClass::AnimatedModelType) {

            //If the object is a standard model then it is modeled with triangles
            _geometryType = GeometryType::Sphere;
        }

        //Hook up to kinematic update for proper physics handling
        _clock->subscribeKinematicsRate(std::bind(&Model::_updateKinematics, this, std::placeholders::_1));

}

Model::~Model() {
    delete _debugShaderProgram;
    delete _shaderProgram;
}

void Model::_updateDraw() {

    //if debugging normals, etc.
    if (_debugMode) {

        //Run debug model shader by allowing the shader to operate on the model
        _debugShaderProgram->runShader(this);
    }

    //Run model shader by allowing the shader to operate on the model
    _shaderProgram->runShader(this);

}

void Model::_updateKeyboard(int key, int x, int y) {
}
void Model::_updateReleaseKeyboard(int key, int x, int y) {
}
void Model::_updateMouse(double x, double y) {
}

void Model::_updateView(Matrix view) {

    _mvp.setView(view); //Receive updates when the view matrix has changed

    //If view changes then change our normal matrix
    _mvp.setNormal(view.inverse().transpose());
}

void Model::_updateProjection(Matrix projection) {
    _mvp.setProjection(projection); //Receive updates when the projection matrix has changed
}

void Model::_updateKinematics(int milliSeconds) {
    //Do kinematic calculations
    _state.update(milliSeconds);

    //Pass position information to model matrix
    Vector4 position = _state.getLinearPosition();
    _geometry.updatePosition(position);
    _mvp.setModel(Matrix::translation(position.getx(), position.gety(), position.getz()));
}

VBO* Model::getVBO() {
    return &_vbo;
}

ModelClass Model::getClassType() {
    return _classId;
}

size_t Model::getArrayCount() {
    if (_classId == ModelClass::AnimatedModelType) {
        return _renderBuffers.getIndices()->size();
    }
    else if (_classId == ModelClass::ModelType) {
        return _renderBuffers.getVertices()->size();
    }
    else {
        return 0;
    }
}

void Model::addTexture(std::string textureName, int stride) {
    _textureStrides.push_back(std::pair<std::string, int>(textureName, stride));
    _textureManager->addTexture(textureName);
}

Texture* Model::getTexture(std::string textureName) {
    return _textureManager->getTexture(textureName);
}

std::vector<std::pair<std::string, int>>& Model::getTextureStrides() {
    return _textureStrides;
}

GeometryType Model::getGeometryType() {
    return _geometryType;
}

void Model::addGeometryTriangle(Triangle triangle) {
    _geometry.addTriangle(triangle);
}

void Model::addGeometrySphere(Sphere sphere) {
    _geometry.addSphere(sphere);
}

std::string Model::_getModelName(std::string name) {
    std::string modelName = name;
    for (int j = 0; j < 3; ++j) { //Strip off the beginning of the file location
        modelName = modelName.substr(modelName.find_first_of("/") + 1);
    }
    modelName = modelName.substr(0, modelName.find_first_of("/"));
    return modelName;
}

Geometry* Model::getGeometry() {
    return &_geometry;
}

MVP* Model::getMVP() {
    return &_mvp;
}

RenderBuffers* Model::getRenderBuffers() {
    return &_renderBuffers;
}

StateVector* Model::getStateVector() {
    return &_state;
}

void Model::setPosition(Vector4 position){
    _state.setLinearPosition(position);
    //Pass position information to model matrix
    _geometry.updatePosition(position);
    _mvp.setModel(Matrix::translation(position.getx(), position.gety(), position.getz()));
}

void Model::setVelocity(Vector4 velocity){
    _state.setLinearVelocity(velocity);
}