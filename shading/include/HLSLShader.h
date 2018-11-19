/*
* HLSLShader is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  HLSLShader class. Basic shader class that is responsible only for compilation and
*  storing the hlsl data of the shader
*/

#pragma once
#include <string>
#include <iostream>

#include <vector>

#include <wrl.h>
#include <d3d12.h>
#include "d3dx12.h"
#include <D3Dcompiler.h>
#include "Shader.h"
#include "ConstantBuffer.h"
#include "RenderTexture.h"
#include "GLIncludes.h"
#include "Uniforms.h"

using namespace Microsoft::WRL;

class Entity;
class Light;
class ShaderBroker;

//Simple shader loading class that should be derived from to create more complex shaders
class HLSLShader : public Shader {

    using InputDescriptors = std::map<std::string, D3D12_SIGNATURE_PARAMETER_DESC>;
    using ResourceDescriptors = std::map<std::string, D3D12_SHADER_INPUT_BIND_DESC>;
    using ConstBuffDescriptors = std::map<std::string, std::vector<D3D12_SHADER_VARIABLE_DESC>>;
protected:
    std::string                           _pipelineShaderName;
    std::vector<D3D12_INPUT_ELEMENT_DESC> _inputLayout;
    ComPtr<ID3D12PipelineState>           _psoState;
    ComPtr<ID3D12RootSignature>           _rootSignature;
    InputDescriptors                      _inputDescriptorTable;
    ResourceDescriptors                   _resourceDescriptorTable;
    ConstBuffDescriptors                  _constBuffDescriptorTable;
    std::map<std::string, UINT>           _resourceIndexes;
    void                                  _queryShaderResources(ComPtr<ID3DBlob> shaderBlob);
    std::wstring                          _stringToLPCWSTR(const std::string& s);
    std::map<std::string, ConstantBuffer*> _constantBuffers;

    ResourceBuffer*                       _indexBuffer;
    D3D12_INDEX_BUFFER_VIEW               _ibv;
public:
    HLSLShader(std::string vertexShaderName, std::string fragmentShaderName = "", 
               std::vector<DXGI_FORMAT>* rtvs = nullptr);
    HLSLShader(const HLSLShader& shader);
    virtual ~HLSLShader();
    void                                  draw(int offset, int instances, int numTriangles);
    void                                  dispatch(int x, int y, int z);
    void                                  build(std::vector<DXGI_FORMAT>* rtvs);
    void                                  updateData(std::string id, void* data);
    void                                  updateData(std::string dataName,
                                                     int textureUnit,
                                                     Texture* exture);
    void                                  updateData(std::string id,
                                                     UINT textureUnit,
                                                     Texture* texture,
                                                     ImageData imageInfo);
    void                                  bindAttributes(VAO* vao);
    void                                  unbindAttributes();
    void                                  bind();
    void                                  unbind();
    static void                           setOM(std::vector<RenderTexture> targets, int width, int height);
    static void                           releaseOM(std::vector<RenderTexture> targets);
};