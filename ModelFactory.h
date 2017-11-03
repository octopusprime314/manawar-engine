#pragma once
#include "AnimatedModel.h"
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
    AnimatedModel* makeAnimatedModel();
	void setViewWrapper(ViewManager* viewManager); //Sets the reference to the view model's event object
};
