#pragma once
#include "Matrix.h"
#include "StateVector.h"
#include <vector>
#include "UpdateInterface.h"
#include "GLIncludes.h"
#include "Shader.h"
#include <iostream>

class Model : public UpdateInterface {
    Matrix _model; //Object and World Space Matrix i.e. how the model is centered around the origin and 
				   //where it is placed in the context of the game World
	Matrix _view;  //View matrix updates from ViewManager/Camera 
	Matrix _projection; //Projection matrix based on ViewManager/Camera
    StateVector _state; //Kinematics
    std::vector<Vector4> _vertices; //Vertices that make up the triangles of the model
    GLuint _bufferContext; //Used as the vbo context
	Shader _shaderProgram; //Container object of the Model's shader
protected:
	void updateKeyboard(unsigned char key, int x, int y); //Do stuff based on keyboard upate
	void updateMouse(int button, int state, int x, int y); //Do stuff based on mouse update
	void updateDraw(); //Do draw stuff
	void updateView(Matrix view); //Get view matrix updates
	void updateProjection(Matrix projection); //Get projection matrix updates

public:
    Model();
};