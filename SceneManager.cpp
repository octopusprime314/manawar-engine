#include "SceneManager.h"

SceneManager::SceneManager(){
	_modelFactory = ModelFactory::instance(); //grab static instance
	
    _modelList.push_back(_modelFactory->makeModel(_viewManager.getEventWrapper())); //Add a model to the scene

	_viewManager.setProjection(); //Initializes projection matrix and broadcasts upate to all listeners

}