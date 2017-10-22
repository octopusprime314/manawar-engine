#pragma once
#include "Model.h"

class ViewManagerEvents;
//ModelFactory is a singleton factory
class ModelFactory{

	static ModelFactory* _factory;
public:
	ModelFactory();
	~ModelFactory();
	static ModelFactory* instance();
	Model* makeModel(ViewManagerEvents* eventWrapper);
};
