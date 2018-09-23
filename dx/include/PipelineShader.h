#pragma once
#include <string>
#include <wrl.h>
#include <d3d12.h>
#include "d3dx12.h"
#include <vector>
#include <D3Dcompiler.h>
#include <map>

using namespace Microsoft::WRL;

class PipelineShader {
    
    std::vector<D3D12_INPUT_ELEMENT_DESC>                 _inputLayout;
    ComPtr<ID3D12PipelineState>                           _psoState;
    ComPtr<ID3D12RootSignature>                           _rootSignature;
    std::map<std::string, D3D12_SIGNATURE_PARAMETER_DESC> _inputDescriptorTable;
    std::map<std::string, D3D12_SHADER_INPUT_BIND_DESC>   _resourceDescriptorTable;
    std::map<std::string, UINT>                           _resourceIndexes;
    void                                                  _queryShaderResources(ComPtr<ID3DBlob> shaderBlob);

public:
    PipelineShader(std::string shader,
                   ComPtr<ID3D12Device> device,
                   DXGI_FORMAT format);

    ComPtr<ID3D12PipelineState>  getPSO();
    ComPtr<ID3D12RootSignature>  getRootSignature();
    std::map<std::string, UINT>& getResourceBindings();
};