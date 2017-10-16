#pragma once
#include "Matrix.h"
#include "StateVector.h"
#include <vector>
#include "UpdateInterface.h"
#include "GLIncludes.h"
#include <iostream>

class Model : public UpdateInterface {
    Matrix _model; //Object and World Space Matrix i.e. how the model is centered around the origin and 
				   //where it is placed in the context of the game World
    StateVector _state; //Kinematics
    std::vector<Vector4> _vertices; //Vertices that make up the triangles of the model
    GLuint _bufferContext; //Used as the vbo context
protected:
	void updateKeyboard(unsigned char key, int x, int y); //Do stuff based on keyboard upate
	void updateMouse(int button, int state, int x, int y); //Do stuff based on mouse update

public:
    Model();
    void render(); //Pushes vertex data onto the stack and loads shader program
};