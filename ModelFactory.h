#pragma once
#include "Model.h"

class ViewManager;
//ModelFactory is a singleton factory
class ModelFactory{

	static ModelFactory* _factory;
	ViewManagerEvents* _viewEventWrapper;
public:
	ModelFactory();
	~ModelFactory();
	static ModelFactory* instance();
	Model* makeModel();
	void setViewWrapper(ViewManager* viewManager); //Sets the reference to the view model's event object
};
