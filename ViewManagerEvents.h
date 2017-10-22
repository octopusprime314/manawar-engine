#pragma once
#include <vector>
#include <functional>
#include "Matrix.h"

class ViewManagerEvents{

	std::vector<std::function<void(Matrix)>> _projectionFuncs;
	std::vector<std::function<void(Matrix)>> _viewFuncs;

public:
	void subscribeToView(std::function<void(Matrix)> func); //Use this call to connect functions to camera/view updates
	void subscribeToProjection(std::function<void(Matrix)> func); //Use this call to connect functions to projection updates
	void updateView(Matrix view); //Blast all subscribers that have overriden the updateView function
	void updateProjection(Matrix view); //Blast all subscribers that have overriden the updateProjection function

};
