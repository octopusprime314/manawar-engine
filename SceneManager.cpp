#include "SceneManager.h"

SceneManager::SceneManager(int* argc, char** argv, unsigned int viewportWidth, unsigned int viewportHeight, float nearPlaneDistance, float farPlaneDistance){
	
	_viewManager = new ViewManager(argc, argv, viewportWidth, viewportHeight);

	_modelFactory = ModelFactory::instance(); //grab static instance

	_modelFactory->setViewWrapper(_viewManager); //Set the reference to the view model event interface
	
    _modelList.push_back(_modelFactory->makeModel()); //Add a model to the scene

	_viewManager->setProjection(viewportWidth, viewportHeight, nearPlaneDistance, farPlaneDistance); //Initializes projection matrix and broadcasts upate to all listeners

	_viewManager->run(); //Enables the glut main loop
}