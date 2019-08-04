#pragma once
#include <string>
#include <wrl.h>
#include <d3d12.h>
#include "d3dx12.h"
#include <vector>
#include <D3Dcompiler.h>
#include <map>

using namespace Microsoft::WRL;
using SignatureMap      = std::map<std::string, D3D12_SIGNATURE_PARAMETER_DESC>;
using ShaderInputMap    = std::map<std::string, D3D12_SHADER_INPUT_BIND_DESC>;
using ShaderResourceMap = std::map<std::string, UINT>;

class PipelineShader {
    
protected:
    std::vector<D3D12_INPUT_ELEMENT_DESC> _inputLayout;
    ComPtr<ID3D12PipelineState>           _psoState;
    ComPtr<ID3D12RootSignature>           _rootSignature;
    SignatureMap                          _inputDescriptorTable;
    ShaderInputMap                        _resourceDescriptorTable;
    ShaderResourceMap                     _resourceIndexes;
    virtual void                          _queryShaderResources(ComPtr<ID3DBlob> shaderBlob);

public:
    PipelineShader();
    PipelineShader(std::string shader,
                   ComPtr<ID3D12Device> device,
                   DXGI_FORMAT format);

    std::wstring                          stringToLPCWSTR(const std::string& s);
    std::map<std::string, UINT>&          getResourceBindings();
    ComPtr<ID3D12RootSignature>           getRootSignature();
    ComPtr<ID3D12PipelineState>           getPSO();
};