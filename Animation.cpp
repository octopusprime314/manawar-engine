#include "Animation.h"
#include "Model.h"
Animation::Animation() : _currentAnimationFrame(0),
    _storedInGPU(false) {

}

Animation::~Animation() {

}

void Animation::addFrame(AnimationFrame* frame, bool sendToGPU) {
    
    if(!sendToGPU){ //Just store in RAM if not loading to gpu
        _frames.push_back(frame);
    }
    else{ //Load the frame into gpu
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

        GLuint vbo[3];

        glGenBuffers(3, &vbo[0]); //Grab three buffer contexts

        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //Load in vertex buffer context
        //Load the vertex data into the current vertex context
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*triBuffSize, flattenVerts, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind

        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]); //Load in normal buffer context
        //Load the normal data into the current normal context
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*triBuffSize, flattenNorms, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind

        glBindBuffer(GL_ARRAY_BUFFER, vbo[2]); //Load in normal debug buffer context
        //Load the normal debug data into the current normal debug context
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*lineBuffSize, flattenNormLines, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind

        //Store buffer contexts
        _frameVerticesVBO.push_back(vbo[0]);
        _frameNormalsVBO.push_back(vbo[1]);
        _frameDebugNormalsVBO.push_back(vbo[2]);

        delete[] flattenVerts;
        delete[] flattenNorms;
        delete[] flattenNormLines;
    }

    _storedInGPU = sendToGPU; //used to notify the model that this particular animation is already in gpu or not
}

void Animation::loadFrame(Model* model){
    
    if(_storedInGPU){

        if (_currentAnimationFrame >= _frameVerticesVBO.size()) {
            _currentAnimationFrame = 0;
        }

        model->setVertexContext(_frameVerticesVBO[_currentAnimationFrame]);
        model->setNormalContext(_frameNormalsVBO[_currentAnimationFrame]);
        model->setNormalDebugContext(_frameDebugNormalsVBO[_currentAnimationFrame]);
    }
    else{

        if (_currentAnimationFrame >= _frames.size()) {
            _currentAnimationFrame = 0;
        }

        AnimationFrame *frame = _frames[_currentAnimationFrame];

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

        glBindBuffer(GL_ARRAY_BUFFER, model->getBufferedVertexContext()); //Load in vertex buffer context
        //Load the vertex data into the current vertex context
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*triBuffSize, flattenVerts, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind

        glBindBuffer(GL_ARRAY_BUFFER, model->getBufferedNormalContext()); //Load in normal buffer context
        //Load the normal data into the current normal context
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*triBuffSize, flattenNorms, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind

        glBindBuffer(GL_ARRAY_BUFFER, model->getBufferedNormalDebugContext()); //Load in normal debug buffer context
        //Load the normal debug data into the current normal debug context
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*lineBuffSize, flattenNormLines, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind

        delete[] flattenVerts;
        delete[] flattenNorms;
        delete[] flattenNormLines;
    }

    _currentAnimationFrame++; //Increment the animation frame counter for next call
}

int Animation::getFrameCount() {
    return _animationFrames;
}
void Animation::setFrames(int animationFrames) {
    _animationFrames = animationFrames;
}