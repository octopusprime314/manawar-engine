#include "AnimatedModel.h"
#include "AnimationShader.h"

AnimatedModel::AnimatedModel(std::string name, ViewManagerEvents* eventWrapper) :
    Model(name, eventWrapper, ModelClass::AnimatedModelType),
    _currentAnimation(0) {

        //First create an animation object
        Animation* animation = AnimationBuilder::buildAnimation();
        addAnimation(animation); //Push animation onto the animated model

        _fbxLoader->loadAnimatedModel(this, _fbxLoader->getScene()->GetRootNode()); //Grab skin animation transforms

        _fbxLoader->buildAnimationFrames(this, animation->getSkins()); //Build up each animation frame's vertices and normals

        //Done with fbx loader so close all handles to fbx scene instance
        delete _fbxLoader;

        std::string modelName = _getModelName(name);
        std::string colliderName = "../models/meshes/";
        colliderName.append(modelName).append("/collider.fbx");
        //Load in geometry fbx object 
        FbxLoader geometryLoader(colliderName);
        //Populate model with fbx file data and recursivelty search with the root node of the scene
        geometryLoader.loadGeometry(this, geometryLoader.getScene()->GetRootNode());

        //Override default shader with a bone animation shader
        _shaderProgram = new AnimationShader("animatedShader");

        _currBones = _animations[_currentAnimation]->getBones();

        auto boneIndexes = _animations[_currentAnimation]->getBoneIndexes();
        auto boneWeights = _animations[_currentAnimation]->getBoneWeights();

        //Now flatten bone indexes and weights out for opengl
        std::vector<int>* indices = _renderBuffers.getIndices();
        size_t boneIndexesBuffSize = indices->size() * 8;
        float* flattenIndexes = new float[boneIndexesBuffSize]; 
        float* flattenWeights = new float[boneIndexesBuffSize]; 

        int i = 0; //iterates through vertices indexes
        for (auto vertexIndex : *indices) {
            auto indexes = (*boneIndexes)[vertexIndex];
            flattenIndexes[i++] = indexes[0];
            flattenIndexes[i++] = indexes[1];
            flattenIndexes[i++] = indexes[2];
            flattenIndexes[i++] = indexes[3];
            flattenIndexes[i++] = indexes[4];
            flattenIndexes[i++] = indexes[5];
            flattenIndexes[i++] = indexes[6];
            flattenIndexes[i++] = indexes[7];
        }
        i = 0; //Reset for normal indexes
        for (auto vertexIndex : *indices) {
            auto weights = (*boneWeights)[vertexIndex];
            flattenWeights[i++] = weights[0];
            flattenWeights[i++] = weights[1];
            flattenWeights[i++] = weights[2];
            flattenWeights[i++] = weights[3];
            flattenWeights[i++] = weights[4];
            flattenWeights[i++] = weights[5];
            flattenWeights[i++] = weights[6];
            flattenWeights[i++] = weights[7];
        }

        glGenBuffers(1, &_indexContext);
        glBindBuffer(GL_ARRAY_BUFFER, _indexContext); //Load in vertex buffer context
        //Load the vertex data into the current vertex context
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*boneIndexesBuffSize, flattenIndexes, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind

        glGenBuffers(1, &_weightContext);
        glBindBuffer(GL_ARRAY_BUFFER, _weightContext); //Load in normal buffer context
        //Load the normal data into the current normal context
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*boneIndexesBuffSize, flattenWeights, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind

        delete[] flattenIndexes;
        delete[] flattenWeights;

        //Hook up to framerate update for proper animation progression
        _clock->subscribeAnimationRate(std::bind(&AnimatedModel::_updateAnimation, this, std::placeholders::_1)); 
}

AnimatedModel::~AnimatedModel() {

    for (auto animation : _animations) {
        delete animation;
    }
}

void AnimatedModel::_updateDraw() {

    //If animation update request was received from an asynchronous event then send vbo to gpu
    if(_animationUpdateRequest){

        //Set bone animation frame
        _currBones = _animations[_currentAnimation]->getBones(); 
        _animations[_currentAnimation]->nextFrame(); //Set animation to the next frame

        _updateLock.lock(); _animationUpdateRequest = false; _updateLock.unlock();
    }

    Model::_updateDraw();
}

void AnimatedModel::_updateKeyboard(unsigned char key, int x, int y) {
    //Update animation 

    //Call base class keyboard handling
    Model::_updateKeyboard(key, x, y);
}

void AnimatedModel::_updateMouse(int button, int state, int x, int y) {
    //Invoke an animation 

    //Call base class keyboard handling
    Model::_updateMouse(button, state, x, y);
}

void AnimatedModel::addAnimation(Animation* animation) {
    _animations.push_back(animation);
}

Animation* AnimatedModel::getAnimation() {
    return _animations.back();
}

void AnimatedModel::_updateAnimation(int milliSeconds){
    //Coordinate loading new animation frame to gpu
    _updateLock.lock(); _animationUpdateRequest = true; _updateLock.unlock();
}

GLuint AnimatedModel::getIndexContext(){
    return _indexContext;
}

GLuint AnimatedModel::getWeightContext(){
    return _weightContext;
}

std::vector<Matrix>* AnimatedModel::getBones(){
    return _currBones;
}

