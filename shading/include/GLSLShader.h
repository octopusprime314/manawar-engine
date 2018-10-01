/*
* GLSLShader is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  GLSLShader class. Basic shader class that is responsible only for compilation and
*  storing the glsl of the shader
*/

#pragma once
#include <string>
#include <iostream>
#include "GLIncludes.h"
#include "Uniforms.h"
#include "Shader.h"

class Entity;
class Light;
class ShaderBroker;

//Simple shader loading class that should be derived from to create more complex shaders
class GLSLShader : public Shader {

protected:
    GLuint       _shaderContext; //keeps track of the shader context
    std::string  _vertexShaderName;
    std::string  _fragmentShaderName;
    Uniforms*    _uniforms;
    unsigned int _compile(char* filename, unsigned int type);
    void         _link(unsigned int vertexSH, unsigned int fragmentSH,
                       unsigned int geomSH, unsigned int computeSH);

public:
    GLSLShader(std::string vertexShaderName, std::string fragmentShaderName = "");
    GLSLShader(const GLSLShader& shader);
    virtual      ~GLSLShader();
    GLint        getShaderContext();
    GLint        getLocation(std::string uniformName);
    Uniforms*    getUniforms();
    ////Updates most mat, vec, etc. data types in glsl
    //void         _shader->updateData(std::string uniformName, 
    //                void* data);
    ////Updates samplers in glsl
    //void         _shader->updateData(std::string uniformName, 
    //                GLuint textureUnit, 
    //                GLuint textureContext);
    ////Updates images in glsl used primarily in compute shaders
    //void         _shader->updateData(std::string uniformName,
    //                GLuint textureUnit,
    //                GLuint textureContext,
    //                ImageData imageInfo);

    void         updateShader(GLSLShader* shader);
    void         draw(int offset, int instances, int numTriangles);
    void         build();
    void         updateData(std::string id, void* data);
    void         updateData(std::string dataName,
                            int textureUnit,
                            Texture* texture);
    void         updateData(std::string id,
                            GLuint textureUnit,
                            GLuint textureContext,
                            ImageData imageInfo);
    void         bindAttributes(VAO* vao);
    void         unbindAttributes();
    void         bind();
    void         unbind();

};