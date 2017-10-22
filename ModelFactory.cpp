#include "ModelFactory.h"
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
Model* ModelFactory::makeModel(ViewManagerEvents* eventWrapper) {
	return new Model(eventWrapper);
}