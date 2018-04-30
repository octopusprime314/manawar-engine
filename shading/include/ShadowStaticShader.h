/*
* ShadowStaticShader is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
* Copyright (c) 2017 Peter Morley.
*
* ReBoot is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, version 3.
*
* ReBoot is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/**
*  ShadowStaticShader class. Derives off of the basic Shader class and is solely
*  responsible for defining which uniforms should be in the shadow static shader model
*  If a uniform does not exist in the shader code there will be an error.
*/

#pragma once
#include "Shader.h"
#include "Model.h"
#include "Light.h"

class ShadowStaticShader : public Shader{

protected:
    GLint       _viewLocation;
    GLint       _modelLocation;
    GLint       _projectionLocation;
public:
    ShadowStaticShader(std::string shaderName);
    virtual		 ~ShadowStaticShader();
	void         runShader(Model* model, Light* light);
    GLint        getViewLocation();
    GLint        getModelLocation();
    GLint        getProjectionLocation();
};
