#pragma once
#include "Matrix.h"
#include "StateVector.h"
#include <vector>
#include "UpdateInterface.h"
#include "GLIncludes.h"
#include "Shader.h"
#include "DebugShader.h"
#include <iostream>

class SimpleContext;

class Model : public UpdateInterface {
    
protected:
    Matrix _model; //Object and World Space Matrix i.e. how the model is centered around the origin and 
                   //where it is placed in the context of the game World
    Matrix _view;  //View matrix updates from ViewManager/Camera 
    Matrix _projection; //Projection matrix based on ViewManager/Camera
    Matrix _normal; //Normal matrix based on inverse transpose matrix of the ViewManager/Camera matrix
    StateVector _state; //Kinematics
    std::vector<Vector4> _vertices; //Vertices that make up the triangles of the model
    std::vector<Vector4> _normals; //Normals that implement how light is shaded onto a model
    GLuint _vertexBufferContext; //Used as the vertex attribute vbo context
    GLuint _normalBufferContext; //Used as the normal attribute vbo context
    GLuint _debugNormalBufferContext; //Used as the debug normal line attribute vbo context
    Shader* _shaderProgram; //Container object of the Model's shader
    DebugShader* _debugShaderProgram; //Container object of the normal line shader
    bool _debugMode; //Runs an extra shader with debug information include normals
    std::vector<Vector4> _normalLineVertices; //Vertex storage for normal line visualization

	void updateKeyboard(unsigned char key, int x, int y); //Do stuff based on keyboard upate
	void updateMouse(int button, int state, int x, int y); //Do stuff based on mouse update
	void updateDraw(); //Do draw stuff
	void updateView(Matrix view); //Get view matrix updates
	void updateProjection(Matrix projection); //Get projection matrix updates

public:
    Model();
	Model(ViewManagerEvents* eventWrapper);
	Matrix getModel();
	Matrix getView();
	Matrix getProjection();
	Matrix getNormal();
	float* getModelBuffer();
	float* getViewBuffer();
	float* getProjectionBuffer();
	float* getNormalBuffer();
	GLuint getVertexContext();
	GLuint getNormalContext();
	GLuint getNormalDebugContext();
	size_t getVertexCount();
	size_t getNormalLineCount();
    void   addVertex(Vector4 vertex);
    void   addNormal(Vector4 normal);
    void   addDebugNormal(Vector4 normal);
};