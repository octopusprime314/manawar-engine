#include "ModelFactory.h"
#include "ViewManager.h"
//Initialize the factory
ModelFactory* ModelFactory::_factory = nullptr;

ModelFactory::ModelFactory() { 
	_factory = nullptr;
}

ModelFactory::~ModelFactory() {
	delete _factory;
}

ModelFactory* ModelFactory::instance() { //Only initializes the static pointer once
	if(_factory == nullptr){
		_factory = new ModelFactory();
	}
	return _factory;
}

void ModelFactory::setViewWrapper(ViewManager* viewManager) { 
	_viewEventWrapper = viewManager->getEventWrapper();
}	

Model* ModelFactory::makeModel(std::string name) {
	return new Model(name, _viewEventWrapper);
}

AnimatedModel* ModelFactory::makeAnimatedModel(std::string name) {
    return new AnimatedModel(name, _viewEventWrapper);
}