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
        std::string colliderName = MESH_LOCATION;
        colliderName.append(modelName).append("/collider.fbx");
        //Load in geometry fbx object 
        FbxLoader geometryLoader(colliderName);
        //Populate model with fbx file data and recursivelty search with the root node of the scene
        geometryLoader.loadGeometry(this, geometryLoader.getScene()->GetRootNode());

        //Override default shader with a bone animation shader
        _shaderProgram = new AnimationShader("animatedShader");

        _currBones = _animations[_currentAnimation]->getBones();

        _vao.createVAO(&_renderBuffers, ModelClass::AnimatedModelType, _animations[_currentAnimation]);

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
void AnimatedModel::_updateReleaseKeyboard(int key, int x, int y) {
}

void AnimatedModel::_updateMouse(double x, double y) {
    //Invoke an animation 

    //Call base class keyboard handling
    Model::_updateMouse(x, y);
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

