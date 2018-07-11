#include "AnimatedModel.h"
#include "AnimationShader.h"
#include "ShaderBroker.h"

AnimatedModel::AnimatedModel(std::string name) :
    Model(name, ModelClass::AnimatedModelType),
    _currentAnimation(0) {

    //First create an animation object
    Animation* animation = AnimationBuilder::buildAnimation();
    addAnimation(animation); //Push animation onto the animated model

    _fbxLoader->loadAnimatedModel(this, _fbxLoader->getScene()->GetRootNode()); //Grab skin animation transforms

    _fbxLoader->buildAnimationFrames(this, animation->getSkins()); //Build up each animation frame's vertices and normals

    //Done with fbx loader so close all handles to fbx scene instance
    delete _fbxLoader;

    std::string modelName = _getModelName(name);
    std::string colliderName = ANIMATED_MESH_LOCATION;
    colliderName.append(modelName).append("/collider.fbx");
    //Load in geometry fbx object
    FbxLoader geometryLoader(colliderName);
    //Populate model with fbx file data and recursivelty search with the root node of the scene
    geometryLoader.loadGeometry(this, geometryLoader.getScene()->GetRootNode());

    //Override default shader with a bone animation shader
    _shaderProgram = static_cast<AnimationShader*>(ShaderBroker::instance()->getShader("animatedShader"));

    _currBones = _animations[_currentAnimation]->getBones();

    _vao.createVAO(&_renderBuffers, ModelClass::AnimatedModelType, _animations[_currentAnimation]);
}

AnimatedModel::~AnimatedModel() {

    
}

void AnimatedModel::updateModel(Model* model) {
    std::lock_guard<std::mutex> lockGuard(_updateLock);

    auto animatedModel = static_cast<AnimatedModel*>(model);
    this->_indexContext = animatedModel->_indexContext;
    this->_weightContext = animatedModel->_weightContext;

    for (auto animation : this->_animations) {
        delete animation;
    }

    this->_animations = animatedModel->_animations;

    Model::updateModel(model);
}

void AnimatedModel ::updateAnimation() {
    //If animation update request was received from an asynchronous event then send vbo to gpu
    if (_animationUpdateRequest) {

        //Set bone animation frame
        _currBones = _animations[_currentAnimation]->getBones();
        _animations[_currentAnimation]->nextFrame(); //Set animation to the next frame

        _updateLock.lock(); _animationUpdateRequest = false; _updateLock.unlock();
    }
}

void AnimatedModel::addAnimation(Animation* animation) {
    _animations.push_back(animation);
}

Animation* AnimatedModel::getAnimation() {
    return _animations.back();
}

void AnimatedModel::triggerNextFrame() {
    //Coordinate loading new animation frame to gpu
    _updateLock.lock(); _animationUpdateRequest = true; _updateLock.unlock();
}

GLuint AnimatedModel::getIndexContext() {
    return _indexContext;
}

GLuint AnimatedModel::getWeightContext() {
    return _weightContext;
}

std::vector<Matrix>* AnimatedModel::getBones() {
    return _currBones;
}

