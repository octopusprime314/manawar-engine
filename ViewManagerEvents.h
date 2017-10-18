#pragma once
#include <vector>
#include <functional>
#include "Matrix.h"

class ViewManagerEvents{

	static std::vector<std::function<void(Matrix)>> _projectionFuncs;
	static std::vector<std::function<void(Matrix)>> _viewFuncs;

public:
	static void subscribeToView(std::function<void(Matrix)> func); //Use this call to connect functions to camera/view updates
	static void subscribeToProjection(std::function<void(Matrix)> func); //Use this call to connect functions to projection updates
	static void updateView(Matrix view); //Blast all subscribers that have overriden the updateView function
	static void updateProjection(Matrix view); //Blast all subscribers that have overriden the updateProjection function

};
