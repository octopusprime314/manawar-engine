#include "Entity.h"
#include "SimpleContext.h"
#include "Model.h"
#include "FrustumCuller.h"

unsigned int Entity::_idGenerator = 0;

Entity::Entity(Model* model, ViewManagerEvents* eventWrapper) :
    UpdateInterface(eventWrapper),
    _clock(MasterClock::instance()),
    _model(model),
    _id(_idGenerator),
    _selected(false),
    _frustumRenderBuffers(new std::vector<RenderBuffers>()) {


    if (_model->getClassType() == ModelClass::AnimatedModelType) {
        _state.setActive(true); //initialize animations to be active
        
        //Hook up to framerate update for proper animation progression
        _clock->subscribeAnimationRate(std::bind(&Entity::_updateAnimation, this, std::placeholders::_1));

        //Test a simple bounding box for animations at first, POC
        _frustumCuller = new FrustumCuller(this, *_model->getAABB());
        _idGenerator++;
    }
    else if (_model->getClassType() == ModelClass::ModelType) {
        _frustumCuller = new FrustumCuller(this, 2000, 4000);
        //Tile the terrain and other static objects in the scene
        _generateVAOTiles();
        _idGenerator++;
    }
    //Hook up to kinematic update for proper physics handling
    _clock->subscribeKinematicsRate(std::bind(&Entity::_updateKinematics, this, std::placeholders::_1));

}

Entity::~Entity() {
}

void Entity::_updateDraw() {

    if (_model->getClassType() == ModelClass::AnimatedModelType) {

        AnimatedModel* animatedModel = static_cast<AnimatedModel*>(_model);
        animatedModel->updateAnimation();
    }
    //Run model shader by allowing the shader to operate on the model
    _model->runShader(this);
}

Model* Entity::getModel() {
    return _model;
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

    //Pass position information to model matrix
    Vector4 position = _state.getLinearPosition();
    _model->getGeometry()->updatePosition(position);
    _mvp.getModelBuffer()[3] = position.getx();
    _mvp.getModelBuffer()[7] = position.gety();
    _mvp.getModelBuffer()[11] = position.getz();
}

void Entity::_updateGameState(int state) {
}

VAOMap Entity::getVAOMapping() {
    return _frustumVAOMapping;
}

MVP* Entity::getMVP() {
    return &_mvp;
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
                renderBuff.addVertex((*vertices)[triangle.first]);
                renderBuff.addVertex((*vertices)[triangle.first + 1]);
                renderBuff.addVertex((*vertices)[triangle.first + 2]);
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

                globalRenderBuffer.addVertex((*vertices)[triangle.first]);
                globalRenderBuffer.addVertex((*vertices)[triangle.first + 1]);
                globalRenderBuffer.addVertex((*vertices)[triangle.first + 2]);
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

    //remove all vao helper info
    _model->getVAO()->clear();
}

std::vector<RenderBuffers>* Entity::getRenderBuffers() {
    return _frustumRenderBuffers;
}

std::vector<VAO*>* Entity::getFrustumVAO() {

    //If not subdividing space using a frustum culler then retrieve whole of geometry
    if (_frustumCuller->getOSP() == nullptr) {
        if (_frustumCuller->getVisibleVAO(this)) {
            return _model->getVAO();
        }
        else {
            return new std::vector<VAO*>(); //empty
        }
    }
    else {
        auto vaoIndexes = _frustumCuller->getVisibleVAOs();
        _frustumVAOs.clear();
        for (auto vaoIndex : vaoIndexes) {
            for (auto vao : _frustumVAOMapping[vaoIndex]) {
                _frustumVAOs.push_back(vao);
            }
        }
        auto addedVAOs = _model->getVAO();
        for (auto vaoIndex : *addedVAOs) {
            _frustumVAOs.push_back(vaoIndex);
        }

        return &_frustumVAOs;
    }
}

FrustumCuller* Entity::getFrustumCuller() {
    return _frustumCuller;
}

void Entity::setPosition(Vector4 position) {
    _state.setLinearPosition(position);
    //Pass position information to model matrix
    _model->getGeometry()->updatePosition(position);

    _prevMVP.setModel(_mvp.getModelMatrix());

    _mvp.setModel(Matrix::translation(position.getx(), position.gety(), position.getz()));
}

void Entity::setVelocity(Vector4 velocity) {
    _state.setLinearVelocity(velocity);
}
