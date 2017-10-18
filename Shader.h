#pragma once
#include <string>
#include <iostream>
#include "GLIncludes.h"


//Simple shader loading class that should be derived from to create more complex shaders
class Shader{

	GLuint _shaderContext; //keeps track of the shader context
	GLhandleARB _loadShader(char* filename, unsigned int type);
	GLint _viewLocation;
	GLint _modelLocation;
	GLint _projectionLocation;
public:
	Shader();
	virtual void compile();
	GLuint getShaderContext();
	GLint getViewLocation();
	GLint getModelLocation();
	GLint getProjectionLocation();
};