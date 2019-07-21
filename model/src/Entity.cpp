#include "Entity.h"
#include "IOEventDistributor.h"
#include "Model.h"
#include "FrustumCuller.h"
#include "GeometryMath.h"
#include "ModelBroker.h"

unsigned int Entity::_idGenerator = 1;

Entity::Entity(Model* model, ViewEvents* eventWrapper, MVP transforms) :
    EventSubscriber(eventWrapper),
    _clock(MasterClock::instance()),
    _model(model),
    _id(_idGenerator),
    _selected(false),
    _frustumRenderBuffers(new std::vector<RenderBuffers>()),
    _gameState(EngineState::getEngineState()),
    _layeredTexture(nullptr) {

    _worldSpaceTransform = transforms.getModelMatrix();
    _mvp.setProjection(transforms.getProjectionMatrix());
    _mvp.setView(transforms.getViewMatrix());

    if (_model->getClassType() == ModelClass::AnimatedModelType) {
        //_state.setActive(true); //initialize animations to be active
        
        //Hook up to framerate update for proper animation progression
        _clock->subscribeAnimationRate(std::bind(&Entity::_updateAnimation, this, std::placeholders::_1));

        //Test a simple bounding box for animations at first, POC
        _frustumCuller = new FrustumCuller(this, *_model->getAABB());
        _idGenerator++;
    }
    else if (_model->getClassType() == ModelClass::ModelType) {
        //_frustumCuller = new FrustumCuller(this, 2000, 4000);
        _frustumCuller = new FrustumCuller(this, 10000000, 100000000);
        //Tile the terrain and other static objects in the scene
        //_generateVAOTiles();
        _idGenerator++;
    }
    //Copy of the base geometry which will then be offset by the entity's world space position
    _worldSpaceGeometry = *_model->getGeometry();
    
    //Hook up to kinematic update for proper physics handling
    _clock->subscribeKinematicsRate(std::bind(&Entity::_updateKinematics, this, std::placeholders::_1));

}

Entity::~Entity() {
}

void Entity::_updateDraw() {

    Matrix inverseViewProjection = ModelBroker::getViewManager()->getView().inverse() *
                                   ModelBroker::getViewManager()->getProjection().inverse();

    if (FrustumCuller::getVisibleAABB(this, inverseViewProjection)) {
        if (_model->getClassType() == ModelClass::AnimatedModelType) {

            AnimatedModel* animatedModel = static_cast<AnimatedModel*>(_model);
            animatedModel->updateAnimation();
        }
        //Run model shader by allowing the shader to operate on the model
        _model->runShader(this);
    }
}

Model* Entity::getModel() {
    auto pos = Vector4(_worldSpaceTransform.getFlatBuffer()[3],
                       _worldSpaceTransform.getFlatBuffer()[7],
                       _worldSpaceTransform.getFlatBuffer()[11]);
    return ModelBroker::instance()->getModel(_model->getName(), pos);
}
Geometry* Entity::getGeometry() {
    return &_worldSpaceGeometry;
}

void Entity::_updateAnimation(int milliSeconds) {
    //Coordinate loading new animation frame to gpu
    if (_model->getClassType() == ModelClass::AnimatedModelType) {

        AnimatedModel* animatedModel = static_cast<AnimatedModel*>(_model);
        animatedModel->triggerNextFrame();
    }
}

void Entity::_updateView(Matrix view) {

    _prevMVP.setView(_mvp.getViewMatrix());

    _mvp.setView(view); //Receive updates when the view matrix has changed

    //If view changes then change our normal matrix
    _mvp.setNormal(view.inverse().transpose());
}

void Entity::_updateProjection(Matrix projection) {
    _mvp.setProjection(projection); //Receive updates when the projection matrix has changed
}

bool Entity::getSelected() {
    return _selected;
}

void Entity::setSelected(bool isSelected) {
    _selected = isSelected;
}

void Entity::_updateKinematics(int milliSeconds) {
    //Do kinematic calculations
    _state.update(milliSeconds);
    _prevMVP.setModel(_mvp.getModelMatrix());

    Vector4 position = _state.getLinearPosition();
    Matrix kinematicTransform = Matrix::translation(position.getx(), position.gety(), position.getz());
    auto totalTransform = kinematicTransform * _worldSpaceTransform;
    
    _worldSpaceGeometry.updateTransform(totalTransform);
    _mvp.setModel(totalTransform);
    
}

void Entity::_updateGameState(EngineStateFlags state) {
    _gameState = state;
}

VAOMap Entity::getVAOMapping() {
    return _frustumVAOMapping;
}

MVP* Entity::getMVP() {
    return &_mvp;
}
void Entity::setMVP(MVP transforms) {
    _worldSpaceTransform = transforms.getModelMatrix();
    _mvp.setProjection(transforms.getProjectionMatrix());
    _mvp.setView(transforms.getViewMatrix());
}
MVP* Entity::getPrevMVP() {
    return &_prevMVP;
}

StateVector* Entity::getStateVector() {
    return &_state;
}

unsigned int Entity::getID() {
    return _id;
}

bool Entity::isID(unsigned int entityID) {

    if (entityID == _id){
        return true;
    }
    else {
        return false;
    }
}

void Entity::_generateVAOTiles() {

    auto* leaves           = _frustumCuller->getOSP()->getOSPLeaves();
    auto renderBuffers     = _model->getRenderBuffers();
    auto vertices          = renderBuffers->getVertices();
    auto normals           = renderBuffers->getNormals();
    auto textures          = renderBuffers->getTextures();
    auto indices           = renderBuffers->getIndices();
    auto textureMapIndices = renderBuffers->getTextureMapIndices();
    auto textureMapNames   = renderBuffers->getTextureMapNames();
    int leafIndex = 1;

    RenderBuffers globalRenderBuffer;

    for (auto textureName : *textureMapNames) {
        globalRenderBuffer.addTextureMapName(textureName);
    }

    unsigned int primitiveOffsetId = 0;
    unsigned int prevPrimitiveOffsetId = primitiveOffsetId;
    for (auto leaf : *leaves) {

        //texture name to triangle mapping
        std::unordered_map<std::string, std::vector<std::pair<int, Triangle*>>> textureTriangleMapper;
        for (std::pair<Entity* const, std::set<std::pair<int, Triangle*>>>& triangleMap : *leaf->getTriangles()) {

            for (auto triangleIndex : triangleMap.second) {
                textureTriangleMapper[(*textureMapNames)[(*textureMapIndices)[triangleIndex.first]]].push_back(triangleIndex);
            }
        }

        prevPrimitiveOffsetId = primitiveOffsetId;

        RenderBuffers renderBuff;

        for (auto textureName : *textureMapNames) {
            renderBuff.addTextureMapName(textureName);
        }
        for (auto triangleMap : textureTriangleMapper) {

            for (auto triangle : triangleMap.second) {
                renderBuff.addVertex(_worldSpaceTransform * (*vertices)[triangle.first]);
                renderBuff.addVertex(_worldSpaceTransform * (*vertices)[triangle.first + 1]);
                renderBuff.addVertex(_worldSpaceTransform * (*vertices)[triangle.first + 2]);
                renderBuff.addNormal((*normals)[triangle.first]);
                renderBuff.addNormal((*normals)[triangle.first + 1]);
                renderBuff.addNormal((*normals)[triangle.first + 2]);
                renderBuff.addTexture((*textures)[triangle.first]);
                renderBuff.addTexture((*textures)[triangle.first + 1]);
                renderBuff.addTexture((*textures)[triangle.first + 2]);

                int index1 = renderBuff.getTextureMapIndex((*textureMapNames)[(*textureMapIndices)[triangle.first]]);
                int index2 = renderBuff.getTextureMapIndex((*textureMapNames)[(*textureMapIndices)[triangle.first + 1]]);
                int index3 = renderBuff.getTextureMapIndex((*textureMapNames)[(*textureMapIndices)[triangle.first + 2]]);
                
                renderBuff.addTextureMapIndex(index1);
                renderBuff.addTextureMapIndex(index2);
                renderBuff.addTextureMapIndex(index3);

                globalRenderBuffer.addVertex(_worldSpaceTransform * (*vertices)[triangle.first]);
                globalRenderBuffer.addVertex(_worldSpaceTransform * (*vertices)[triangle.first + 1]);
                globalRenderBuffer.addVertex(_worldSpaceTransform * (*vertices)[triangle.first + 2]);
                globalRenderBuffer.addNormal((*normals)[triangle.first]);
                globalRenderBuffer.addNormal((*normals)[triangle.first + 1]);
                globalRenderBuffer.addNormal((*normals)[triangle.first + 2]);
                globalRenderBuffer.addTexture((*textures)[triangle.first]);
                globalRenderBuffer.addTexture((*textures)[triangle.first + 1]);
                globalRenderBuffer.addTexture((*textures)[triangle.first + 2]);

                int globalIndex1 = globalRenderBuffer.getTextureMapIndex((*textureMapNames)[(*textureMapIndices)[triangle.first]]);
                int globalIndex2 = globalRenderBuffer.getTextureMapIndex((*textureMapNames)[(*textureMapIndices)[triangle.first + 1]]);
                int globalIndex3 = globalRenderBuffer.getTextureMapIndex((*textureMapNames)[(*textureMapIndices)[triangle.first + 2]]);
                
                globalRenderBuffer.addTextureMapIndex(globalIndex1);
                globalRenderBuffer.addTextureMapIndex(globalIndex2);
                globalRenderBuffer.addTextureMapIndex(globalIndex3);

                primitiveOffsetId++;
            }
        }
        if (renderBuff.getVertices()->size() > 0) {

            auto textureIndices = renderBuff.getTextureMapIndices();
            auto textureNames = renderBuff.getTextureMapNames();

            int textureIndexCount = 0;
            int prevTextureIndex = (*textureIndices)[0];
            int previousCount = 0;
            for (int i = 0; i < textureIndices->size(); i++) {
                int textureIndex = (*textureIndices)[i];

                if (textureIndex != prevTextureIndex) {
                    auto textureName = (*textureNames)[prevTextureIndex];

                    _frustumVAOs.push_back(new VAO());
                    _frustumVAOs.back()->createVAO(&renderBuff, previousCount, textureIndexCount);

                    _frustumVAOs.back()->addTextureStride(
                        std::pair<std::string, int>(textureName, textureIndexCount));

                    _frustumVAOs.back()->setPrimitiveOffsetId(prevPrimitiveOffsetId + (previousCount / 3));

                    _frustumVAOMapping[leafIndex].push_back(_frustumVAOs[_frustumVAOs.size() - 1]);

                    previousCount += textureIndexCount;
                    textureIndexCount = 0;
                }

                if (i == textureIndices->size() - 1) {
                    auto textureName = (*textureNames)[textureIndex];

                    _frustumVAOs.push_back(new VAO());
                    _frustumVAOs.back()->createVAO(&renderBuff, previousCount, textureIndexCount);

                    _frustumVAOs.back()->addTextureStride(
                        std::pair<std::string, int>(textureName, textureIndexCount));

                    _frustumVAOs.back()->setPrimitiveOffsetId(prevPrimitiveOffsetId + (previousCount / 3));

                    _frustumVAOMapping[leafIndex].push_back(_frustumVAOs[_frustumVAOs.size() - 1]);
                }

                textureIndexCount++;
                prevTextureIndex = textureIndex;
            }
        }
        leafIndex++;
    }

    _frustumRenderBuffers->push_back(globalRenderBuffer);

}

std::vector<RenderBuffers>* Entity::getRenderBuffers() {
    return _frustumRenderBuffers;
}

Matrix Entity::getWorldSpaceTransform() {
    return _worldSpaceTransform;
}

LayeredTexture* Entity::getLayeredTexture() {
    return _layeredTexture;
}

void Entity::setLayeredTexture(LayeredTexture* layeredTexture) {
    _layeredTexture = layeredTexture;
}

std::vector<VAO*>* Entity::getFrustumVAO() {
    auto pos = Vector4(_worldSpaceTransform.getFlatBuffer()[3],
                       _worldSpaceTransform.getFlatBuffer()[7],
                       _worldSpaceTransform.getFlatBuffer()[11]);
    auto model = ModelBroker::instance()->getModel(_model->getName(), pos);

    if (_gameState.frustumVisualEnabled) {
        auto vaoIndexes = _frustumCuller->getVisibleVAOs();
        _frustumVAOs.clear();
        for (auto vaoIndex : vaoIndexes) {
            for (auto vao : _frustumVAOMapping[vaoIndex]) {
                _frustumVAOs.push_back(vao);
            }
        }
    }
    else {
        _frustumVAOs.clear();
        auto addedVAOs = model->getVAO();
        //Do not add the original non frustum culled vao that needs to be used for shadows only
        int i = 0;
        for (auto vaoIndex : *addedVAOs) {
            _frustumVAOs.push_back(vaoIndex);
            i++;
        }
    }
    return &_frustumVAOs;

}

FrustumCuller* Entity::getFrustumCuller() {
    return _frustumCuller;
}

void Entity::setPosition(Vector4 position) {

    Matrix kinematicTransform = Matrix::translation(position.getx(), position.gety(), position.getz());
    auto totalTransform = kinematicTransform * _worldSpaceTransform;
    Vector4 pos = Vector4(totalTransform.getFlatBuffer()[3],
        totalTransform.getFlatBuffer()[7],
        totalTransform.getFlatBuffer()[11]);

    _state.setLinearPosition(pos);
    _worldSpaceGeometry.updateTransform(totalTransform);

    _prevMVP.setModel(_mvp.getModelMatrix());

    _mvp.setModel(totalTransform);
}

void Entity::setVelocity(Vector4 velocity) {
    _state.setLinearVelocity(velocity);
}