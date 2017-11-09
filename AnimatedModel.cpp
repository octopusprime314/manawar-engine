#include "AnimatedModel.h"

AnimatedModel::AnimatedModel(std::string name, ViewManagerEvents* eventWrapper) :
    Model(name, eventWrapper, ModelClass::AnimatedModelType),
    _currentAnimation(0) {

    _fbxLoader->loadAnimatedModel(this, _fbxLoader->getScene()->GetRootNode()); //Grab skin animation transforms

    //Done with fbx loader so close all handles to fbx scene instance
    delete _fbxLoader;
}

AnimatedModel::~AnimatedModel() {

    for (auto animation : _animations) {
        delete animation;
    }
}

void AnimatedModel::updateDraw() {

    //Load in the next frame of the animation
    _loadFrame(_animations[_currentAnimation]->getNextFrame());

    Model::updateDraw();
}

void AnimatedModel::updateKeyboard(unsigned char key, int x, int y) {
    //Update animation 

    //Call base class keyboard handling
    Model::updateKeyboard(key, x, y);
}

void AnimatedModel::updateMouse(int button, int state, int x, int y) {
    //Invoke an animation 

    //Call base class keyboard handling
    Model::updateMouse(button, state, x, y);
}

void AnimatedModel::addAnimation(Animation* animation) {
    _animations.push_back(animation);
}

void AnimatedModel::addFrame(AnimationFrame* frame) {
    _animations.back()->addFrame(frame);
}

Animation* AnimatedModel::getAnimation() {
    return _animations.back();
}

void AnimatedModel::_loadFrame(AnimationFrame* frame) {

    auto frameVertices = frame->getVertices();
    auto frameNormals = frame->getNormals();
    auto frameDebugNormals = frame->getDebugNormals();

    //Now flatten vertices and normals out for opengl
    size_t triBuffSize = frameVertices->size() * 3;
    float* flattenVerts = new float[triBuffSize]; //Only include the x y and z values not w
    float* flattenNorms = new float[triBuffSize]; //Only include the x y and z values not w, same size as vertices
    size_t lineBuffSize = frameDebugNormals->size() * 3;
    float* flattenNormLines = new float[lineBuffSize]; //Only include the x y and z values not w, flat line data

    int i = 0; //iterates through vertices indexes
    for (auto vertex : *frameVertices) {
        float *flat = vertex.getFlatBuffer();
        flattenVerts[i++] = flat[0];
        flattenVerts[i++] = flat[1];
        flattenVerts[i++] = flat[2];
    }
    i = 0; //Reset for normal indexes
    for (auto normal : *frameNormals) {
        float *flat = normal.getFlatBuffer();
        flattenNorms[i++] = flat[0];
        flattenNorms[i++] = flat[1];
        flattenNorms[i++] = flat[2];
    }
    i = 0; //Reset for normal line indexes
    for (auto normalLine : *frameDebugNormals) {
        float *flat = normalLine.getFlatBuffer();
        flattenNormLines[i++] = flat[0];
        flattenNormLines[i++] = flat[1];
        flattenNormLines[i++] = flat[2];
    }

    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferContext); //Load in vertex buffer context
    //Load the vertex data into the current vertex context
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*triBuffSize, flattenVerts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind

    glBindBuffer(GL_ARRAY_BUFFER, _normalBufferContext); //Load in normal buffer context
    //Load the normal data into the current normal context
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*triBuffSize, flattenNorms, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind

    glBindBuffer(GL_ARRAY_BUFFER, _debugNormalBufferContext); //Load in normal debug buffer context
    //Load the normal debug data into the current normal debug context
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*lineBuffSize, flattenNormLines, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind

    delete[] flattenVerts;
    delete[] flattenNorms;
    delete[] flattenNormLines;
}