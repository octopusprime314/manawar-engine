#include "Model.h"
#include "SimpleContext.h"
#include "FbxLoader.h"
#include "ShaderBroker.h"
#include "ModelBroker.h"

TextureBroker* Model::_textureManager = TextureBroker::instance();

Model::Model(std::string name, ModelClass classId) :
    _isInstanced(false),
    _classId(classId),
    _name(name),
    _fbxLoader(new FbxLoader((classId == ModelClass::ModelType ? 
        STATIC_MESH_LOCATION : ANIMATED_MESH_LOCATION) + name)) {

    _vao.push_back(new VAO());
    //Populate model with fbx file data and recursivelty search with the root node of the scene
    _fbxLoader->loadModel(this, _fbxLoader->getScene()->GetRootNode());
    //Create vao contexts
    if (classId == ModelClass::ModelType)  {
        if (name.find("landscape") != std::string::npos) {
            std::vector<Entity*> list = { new Entity(this, ModelBroker::getViewManager()->getEventWrapper()) };
            ModelBroker::_frustumCuller = new FrustumCuller(list);

            auto* leaves = ModelBroker::_frustumCuller->getOSP()->getOSPLeaves();//getFrustumLeaves();
            auto vertices = _renderBuffers.getVertices();
            auto normals = _renderBuffers.getNormals();
            auto textures = _renderBuffers.getTextures();
            auto indices = _renderBuffers.getIndices();
            auto textureMapIndices = _renderBuffers.getTextureMapIndices();
            auto textureMapNames = _renderBuffers.getTextureMapNames();
            int leafIndex = 1;
            for (auto leaf : *leaves) {
                RenderBuffers renderBuff;
                for (std::pair<Entity* const, std::set<std::pair<int, Triangle*>>>& triangleMap : *leaf->getTriangles()) {

                    for (auto triangleIndex : triangleMap.second) {
                        renderBuff.addVertex((*vertices)[triangleIndex.first]);
                        renderBuff.addVertex((*vertices)[triangleIndex.first + 1]);
                        renderBuff.addVertex((*vertices)[triangleIndex.first + 2]);
                        renderBuff.addNormal((*normals)[triangleIndex.first]);
                        renderBuff.addNormal((*normals)[triangleIndex.first + 1]);
                        renderBuff.addNormal((*normals)[triangleIndex.first + 2]);
                        renderBuff.addTexture((*textures)[triangleIndex.first]);
                        renderBuff.addTexture((*textures)[triangleIndex.first + 1]);
                        renderBuff.addTexture((*textures)[triangleIndex.first + 2]);

                        renderBuff.addTextureMapName((*textureMapNames)[(*textureMapIndices)[triangleIndex.first]]);
                        renderBuff.addTextureMapName((*textureMapNames)[(*textureMapIndices)[triangleIndex.first + 1]]);
                        renderBuff.addTextureMapName((*textureMapNames)[(*textureMapIndices)[triangleIndex.first + 2]]);
                    }

                    for (auto triangleIndex : triangleMap.second) {
                        int index1 = renderBuff.getTextureMapIndex((*textureMapNames)[(*textureMapIndices)[triangleIndex.first]]);
                        int index2 = renderBuff.getTextureMapIndex((*textureMapNames)[(*textureMapIndices)[triangleIndex.first + 1]]);
                        int index3 = renderBuff.getTextureMapIndex((*textureMapNames)[(*textureMapIndices)[triangleIndex.first + 2]]);

                        renderBuff.addTextureMapIndex(index1);
                        renderBuff.addTextureMapIndex(index2);
                        renderBuff.addTextureMapIndex(index3);
                    }
                }
                if (renderBuff.getVertices()->size() > 0) {
                    
                    auto textureIndices = renderBuff.getTextureMapIndices();
                    auto textureNames   = renderBuff.getTextureMapNames();

                    int textureIndexCount = 0;
                    int prevTextureIndex = 0;
                    int previousCount = 0;
                    for (int i = 0; i < textureIndices->size(); i++) {
                        int textureIndex = (*textureIndices)[i];
                        
                        if (textureIndex != prevTextureIndex) {
                            auto textureName = (*textureNames)[prevTextureIndex];
                            
                            _vao.push_back(new VAO());
                            _vao[_vao.size() - 1]->createVAO(&renderBuff, previousCount, textureIndexCount);

                            _vao[_vao.size() - 1]->addTextureStride(
                                std::pair<std::string, int>(textureName, textureIndexCount));

                            _frustumVBOMapping[leafIndex].push_back(_vao[_vao.size() - 1]);

                            previousCount += textureIndexCount;
                            textureIndexCount = 0;
                        }

                        if (i == textureIndices->size() - 1) {
                            auto textureName = (*textureNames)[textureIndex];
                            
                            _vao.push_back(new VAO());
                            _vao[_vao.size() - 1]->createVAO(&renderBuff, previousCount, textureIndexCount);

                            _vao[_vao.size() - 1]->addTextureStride(
                                std::pair<std::string, int>(textureName, textureIndexCount));

                            _frustumVBOMapping[leafIndex].push_back(_vao[_vao.size() - 1]);
                        }

                        textureIndexCount++;
                        prevTextureIndex = textureIndex;
                    }
                }
                leafIndex++;
            }
        }
        else {
            _vao[0]->createVAO(&_renderBuffers, _classId);
        }
    }

    //If class is generic model then deallocate fbx object,
    //otherwise let derived class clean up _fbxLoader object
    //because the derived class may need to access data from it still
    if (_classId == ModelClass::ModelType) {

        //Load default shader
        _shaderProgram = static_cast<StaticShader*>(ShaderBroker::instance()->getShader("staticShader"));

        //If the object is a standard model then it is modeled with triangles
        _geometryType = GeometryType::Triangle;

        //delete _fbxLoader;

        std::string modelName = _getModelName(name);
        std::string colliderName = STATIC_MESH_LOCATION;
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
}

Model::~Model() {
}

void Model::runShader(Entity* entity) {
    std::lock_guard<std::mutex> lockGuard(_updateLock);
    _shaderProgram->runShader(entity);
}

void Model::addVAO(ModelClass classType) {
    _vao[_vao.size() - 1]->createVAO(&_renderBuffers, classType);
}

void Model::updateModel(Model* model) {
    std::lock_guard<std::mutex> lockGuard(_updateLock);
    this->_geometry       = model->_geometry;
    this->_vao            = model->_vao;
}

std::vector<VAO*>* Model::getVAO() {
    return &_vao;
}

std::vector<VAO*>* Model::getFrustumVAO() {
    if (_name.find("landscape") != std::string::npos) {
        
        auto vboIndexes = ModelBroker::_frustumCuller->getVisibleVBOs();
        _frustumVAOs.clear();
        for (auto vboIndex : vboIndexes) {
            for (auto vbo : _frustumVBOMapping[vboIndex]) {
                _frustumVAOs.push_back(vbo);
            }
        }
        return &_frustumVAOs;
    }
    else {
        return &_vao;
    }
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
        std::cout << "What class is this????" << std::endl;
        return 0;
    }
}

void Model::addTexture(std::string textureName, int stride) {
    _vao[_vao.size() - 1]->addTextureStride(std::pair<std::string, int>(textureName, stride));
    _textureManager->addTexture(textureName);
}

void Model::addLayeredTexture(std::vector<std::string> textureNames, int stride) {

    //Encode layered into string to notify shader what type of texture is used
    std::string sumString = "Layered";
    for (auto& str : textureNames) {
        sumString += str;
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
    modelName = modelName.substr(0, modelName.find_first_of("/"));
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
    _instances = static_cast<int>(offsets.size());
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
