#include "PipelineShader.h"
//Forward decls
std::wstring stringToLPCWSTR(const std::string& s);


PipelineShader::PipelineShader(std::string shader,
    ComPtr<ID3D12Device> device,
    DXGI_FORMAT format) {

    auto shaderString = stringToLPCWSTR(shader);

    ComPtr<ID3DBlob> compiledVS;
    ComPtr<ID3DBlob> errorsVS;

    HRESULT result = D3DCompileFromFile(shaderString.c_str(), 0,
        D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_5_0", 0, 0,
        compiledVS.GetAddressOf(), errorsVS.GetAddressOf());

    ComPtr<ID3DBlob> compiledPS;
    ComPtr<ID3DBlob> errorsPS;
    result = D3DCompileFromFile(shaderString.c_str(), 0,
        D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_5_0", 0, 0,
        compiledPS.GetAddressOf(), errorsPS.GetAddressOf());


    // Query shaders to properly build root signature

    _queryShaderResources(compiledVS);
    _queryShaderResources(compiledPS);

    CD3DX12_ROOT_PARAMETER* RP = new CD3DX12_ROOT_PARAMETER[_resourceDescriptorTable.size()];
    CD3DX12_DESCRIPTOR_RANGE srvTableRange; //reuse
    CD3DX12_DESCRIPTOR_RANGE samplerTableRange; //reuse

    int i = 0;
    for (auto resource : _resourceDescriptorTable) {
        if (resource.second.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER) {
            RP[i].InitAsConstantBufferView(0);
        }
        else if (resource.second.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_SAMPLER) {
            samplerTableRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);
            RP[i].InitAsDescriptorTable(1, &samplerTableRange, D3D12_SHADER_VISIBILITY_PIXEL);
        }
        else if (resource.second.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE) {
            srvTableRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
            RP[i].InitAsDescriptorTable(1, &srvTableRange, D3D12_SHADER_VISIBILITY_PIXEL);
        }
        _resourceIndexes[resource.second.Name] = i;
        i++;
    }

    ComPtr<ID3DBlob> pOutBlob, pErrorBlob;
    CD3DX12_ROOT_SIGNATURE_DESC descRootSignature;
    descRootSignature.Init(static_cast<UINT>(_resourceIndexes.size()),
        &RP[0],
        0,
        nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    D3D12SerializeRootSignature(&descRootSignature,
        D3D_ROOT_SIGNATURE_VERSION_1,
        pOutBlob.GetAddressOf(),
        pErrorBlob.GetAddressOf());

    if (pErrorBlob) {
        OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
    }

    device->CreateRootSignature(1,
        pOutBlob->GetBufferPointer(),
        pOutBlob->GetBufferSize(),
        IID_PPV_ARGS(_rootSignature.GetAddressOf()));

    _inputLayout.push_back({
        "POSITION",
        0,
        DXGI_FORMAT_R32G32B32_FLOAT,
        0, 
        0,
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
        0
        });

    _inputLayout.push_back({
        "UV",
        0,
        DXGI_FORMAT_R32G32_FLOAT,
        0,
        12,
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
        0
        });

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso;
    ZeroMemory(&pso, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    pso.InputLayout = { _inputLayout.data(), static_cast<UINT>(_inputLayout.size()) };
    pso.pRootSignature = _rootSignature.Get();
    pso.VS = {
        reinterpret_cast<BYTE*>(compiledVS->GetBufferPointer()),
        compiledVS->GetBufferSize()
    };
    pso.PS = {
        reinterpret_cast<BYTE*>(compiledPS->GetBufferPointer()),
        compiledPS->GetBufferSize()
    };
    auto rasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    rasterizerState.CullMode = D3D12_CULL_MODE_NONE; //disabled culling 
    pso.RasterizerState = rasterizerState;
    pso.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pso.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    pso.SampleMask = UINT_MAX;
    pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pso.NumRenderTargets = 2;
    pso.RTVFormats[0] = format;
    pso.RTVFormats[1] = format;
    pso.SampleDesc.Count = 1;
    pso.SampleDesc.Quality = 0;
    pso.DSVFormat = DXGI_FORMAT_D32_FLOAT;

    result = device->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&_psoState));
}

void PipelineShader::_queryShaderResources(ComPtr<ID3DBlob> shaderBlob) {
    
    ID3D12ShaderReflection* reflectionInterface;
    D3DReflect(shaderBlob->GetBufferPointer(), 
        shaderBlob->GetBufferSize(), 
        IID_ID3D12ShaderReflection, 
        (void**)&reflectionInterface);

    HRESULT result = S_OK;
    
    for (int i = 0; result == S_OK; i++) {
        D3D12_SIGNATURE_PARAMETER_DESC varDesc;
        result = reflectionInterface->GetInputParameterDesc(i, &varDesc);
        if (result == S_OK) {
            _inputDescriptorTable[varDesc.SemanticName] = varDesc;
        }
    }
    result = S_OK;
    for (int i = 0; result == S_OK; i++) {
        D3D12_SHADER_INPUT_BIND_DESC resourceDesc;
        result = reflectionInterface->GetResourceBindingDesc(i, &resourceDesc);
        if (result == S_OK) {
            _resourceDescriptorTable[resourceDesc.Name] = resourceDesc;
        }
    }
}

ComPtr<ID3D12PipelineState> PipelineShader::getPSO() {
    return _psoState;
}

ComPtr<ID3D12RootSignature> PipelineShader::getRootSignature() {
    return _rootSignature;
}

std::map<std::string, UINT>& PipelineShader::getResourceBindings() {
    return _resourceIndexes;
}

std::wstring stringToLPCWSTR(const std::string& s) {
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}
