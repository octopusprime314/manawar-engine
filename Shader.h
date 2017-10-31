#pragma once
#include <string>
#include <iostream>
#include "GLIncludes.h"

class Model;

//Simple shader loading class that should be derived from to create more complex shaders
class Shader{
protected:
	GLuint _shaderContext; //keeps track of the shader context
	//GLhandleARB _loadShader(char* filename, unsigned int type);
	GLint _viewLocation;
	GLint _modelLocation;
	GLint _projectionLocation;
	GLint _normalLocation;
	GLhandleARB _compile(char* filename, unsigned int type);
	void _link(GLhandleARB vertexShaderHandle, GLhandleARB fragmentShaderHandle);
public:
	Shader();
	virtual void build();
	virtual void runShader(Model* model);
	GLuint getShaderContext();
	GLint getViewLocation();
	GLint getModelLocation();
	GLint getProjectionLocation();
	GLint getNormalLocation();
};
