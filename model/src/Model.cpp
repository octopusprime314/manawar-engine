#include "Model.h"
#include "IOEventDistributor.h"
#include "FbxLoader.h"
#include "ShaderBroker.h"
#include "ModelBroker.h"
#include "FrustumCuller.h"

TextureBroker* Model::_textureManager = TextureBroker::instance();

Model::Model(std::string name,
             ModelClass classId) :
    _isInstanced(false),
    _classId(    classId),
    _name(       name.substr(name.find_last_of("/") + 1)),
    _fbxLoader(  new FbxLoader(name)) {

    //If the model loaded is a scene then all of the model instances have been added as entities
    //in the fbx constructor call so jump out of model constructor
    if (name.find("scene") != std::string::npos) {
        return;
    }

    //trim .fbx off
    _name = _name.substr(0, _name.size() - 4);
    _vao.push_back(new VAO());

    //Populate model with fbx file data and recursivelty search with the root node of the scene
    _fbxLoader->loadModel(this, _fbxLoader->getScene()->GetRootNode());
    _vao.back()->setPrimitiveOffsetId(0);

    //If class is generic model then deallocate fbx object,
    //otherwise let derived class clean up _fbxLoader object
    //because the derived class may need to access data from it still
    if (_classId == ModelClass::ModelType) {

        //Load default shader
        _shaderProgram    = static_cast<StaticShader*>(ShaderBroker::instance()->getShader("staticShader"));

        if (_name.find("tile") != std::string::npos) {
            //If the object is a standard model then it is modeled with triangles
            _geometryType = GeometryType::Triangle;
        }
        else {
            _geometryType = GeometryType::Sphere;
        }
        std::string colliderName = name.substr(0, name.find_last_of("/") + 1) + "collider.fbx";
        //Load in geometry fbx object
        FbxLoader geometryLoader(colliderName);
        //Populate model with fbx file data and recursivelty search with the root node of the scene
        geometryLoader.loadGeometry(this, geometryLoader.getScene()->GetRootNode());

        //Still need to create this vao soley for shadows
        _vao[0]->createVAO(&_renderBuffers, ModelClass::ModelType);
        _fbxLoader->buildCollisionAABB(this);
    }
    else if (_classId == ModelClass::AnimatedModelType) {

        //If the object is a standard model then it is modeled with triangles
        _geometryType = GeometryType::Sphere;
    }
}

Model::~Model() {
}

void Model::runShader(Entity* entity) {
    std::lock_guard<std::mutex> lockGuard(_updateLock);
    _shaderProgram->runShader(entity);
}

void Model::addVAO(ModelClass classType) {
    _vao[_vao.size() - 1]->createVAO(&_renderBuffers, classType);
    _vao.back()->setPrimitiveOffsetId(0);
}

void Model::updateModel(Model* model) {
    std::lock_guard<std::mutex> lockGuard(_updateLock);
    this->_geometry = model->_geometry;
    this->_vao      = model->_vao;
}

std::vector<VAO*>* Model::getVAO() {
    return &_vao;
}

ModelClass Model::getClassType() {
    return _classId;
}

std::string Model::getName() {
    return _name;
}

size_t Model::getArrayCount() {
    if (_classId == ModelClass::AnimatedModelType) {
        return _renderBuffers.getIndices()->size();
    }
    else if (_classId == ModelClass::ModelType) {
        return _renderBuffers.getVertices()->size();
    }
    else {
        std::cout << "What class is this????" << std::endl;
        return 0;
    }
}

void Model::setAABB(Cube* aabbCube) {
    _aabbCube = aabbCube;
}
Cube* Model::getAABB() {
    return _aabbCube;
}

void Model::setGfxAABB(Cube* gfxAABB) {
    _gfxAABB = gfxAABB;
}
Cube* Model::getGfxAABB() {
    return _gfxAABB;
}

std::vector<std::string> Model::getTextureNames() {
    return _textureRecorder;
}

void Model::addTexture(std::string textureName,
                       int         stride) {
    _vao[_vao.size() - 1]->addTextureStride(std::pair<std::string, int>(textureName, stride));
    _textureManager->addTexture(textureName);
    _textureRecorder.push_back( textureName);
}

void Model::addLayeredTexture(std::vector<std::string> textureNames,
                              int                      stride) {

    //Encode layered into string to notify shader what type of texture is used
    std::string sumString = "Layered";
    for (auto& str : textureNames) {
        sumString += str;
        _textureRecorder.push_back(str);
    }
    //Create sum string for later identification
    _vao[_vao.size() - 1]->addTextureStride(std::pair<std::string, int>(sumString, stride));
    _textureManager->addLayeredTexture(textureNames);
}

AssetTexture* Model::getTexture(std::string textureName) {
    return _textureManager->getTexture(textureName);
}

LayeredTexture* Model::getLayeredTexture(std::string textureName) {
    return _textureManager->getLayeredTexture(textureName);
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
    modelName             = modelName.substr(0, modelName.find_first_of("/"));
    return modelName;
}

Geometry* Model::getGeometry() {
    return &_geometry;
}

RenderBuffers* Model::getRenderBuffers() {
    return &_renderBuffers;
}

void Model::setInstances(std::vector<Vector4> offsets) {
    _isInstanced = true;
    int i = 0;
    for (auto& offset : offsets) {
        _offsets[i++] = offset.getx();
        _offsets[i++] = offset.gety();
        _offsets[i++] = offset.getz();
    }
    _instances   = static_cast<int>(offsets.size());
}

bool Model::getIsInstancedModel() {
    return _isInstanced;
}

float* Model::getInstanceOffsets() {
    return _offsets;
}

FbxLoader* Model::getFbxLoader() {
    return _fbxLoader;
}
