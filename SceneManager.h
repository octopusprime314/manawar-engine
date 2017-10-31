#pragma once
#include "ViewManager.h"
#include "ModelFactory.h"
#include <vector>

class SceneManager{
	ViewManager* _viewManager; //manages the view/camera matrix from the user's perspective
	std::vector<Model*> _modelList; //Contains models active in scene
	ModelFactory* _modelFactory; //Creates new models
public:
	SceneManager(int* argc, char** argv, unsigned int viewportWidth, unsigned int viewportHeight, float nearPlaneDistance, float farPlaneDistance);
};