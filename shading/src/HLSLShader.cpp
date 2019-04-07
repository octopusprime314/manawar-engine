#include "HLSLShader.h"
#include "Entity.h"
#include "Light.h"
#include "EngineManager.h"
#include <iostream>
#include <fstream>
#include "ShaderBroker.h"
#include "DXLayer.h"
#include <vector>

// You can hit this in a debugger.
// Set to 'true' to printf every shader that is linked or compiled.
static volatile bool g_VerboseShaders = false;

HLSLShader::HLSLShader(std::string pipelineShaderName, std::string fragmentShaderName,
                       std::vector<DXGI_FORMAT>* rtvs) {
    //set vertex name
    _pipelineShaderName = pipelineShaderName;
    //build it
    build(rtvs);

    UINT indexBytes = static_cast<UINT>(4) * sizeof(UINT);
    int indices[] = { 0,1,2,3 };
    _indexBuffer = new ResourceBuffer(indices, indexBytes,
        DXLayer::instance()->getCmdList(), DXLayer::instance()->getDevice());

    _ibv.BufferLocation = _indexBuffer->getGPUAddress();
    _ibv.Format = DXGI_FORMAT_R32_UINT;
    _ibv.SizeInBytes = indexBytes;
}

HLSLShader::~HLSLShader() {

}

HLSLShader::HLSLShader(const HLSLShader& shader) {
    *this = shader;
}

std::wstring HLSLShader::_stringToLPCWSTR(const std::string& s) {
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

void HLSLShader::build(std::vector<DXGI_FORMAT>* rtvs) {


    auto device = DXLayer::instance()->getDevice();

    std::string shadersLocation = SHADERS_LOCATION;
    shadersLocation += "hlsl/";

    std::string fileName = shadersLocation + _pipelineShaderName;
    fileName.append(".hlsl");

    auto shaderString = _stringToLPCWSTR(fileName);

    ComPtr<ID3DBlob> compiledVS;
    ComPtr<ID3DBlob> errorsVS;

    HRESULT vsResult = D3DCompileFromFile(shaderString.c_str(), 0,
        D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_5_0", 0, 0,
        compiledVS.GetAddressOf(), errorsVS.GetAddressOf());

    if (errorsVS) {
        OutputDebugStringA((char*)errorsVS->GetBufferPointer());
    }

    ComPtr<ID3DBlob> compiledPS;
    ComPtr<ID3DBlob> errorsPS;
    HRESULT psResult = D3DCompileFromFile(shaderString.c_str(), 0,
        D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_5_0", 0, 0,
        compiledPS.GetAddressOf(), errorsPS.GetAddressOf());

    if (errorsPS) {
        OutputDebugStringA((char*)errorsPS->GetBufferPointer());
    }

    ComPtr<ID3DBlob> compiledCS;
    ComPtr<ID3DBlob> errorsCS;
    HRESULT csResult = D3DCompileFromFile(shaderString.c_str(), 0,
        D3D_COMPILE_STANDARD_FILE_INCLUDE, "CS", "cs_5_0", 0, 0,
        compiledCS.GetAddressOf(), errorsCS.GetAddressOf());

    if (csResult) {
        OutputDebugStringA((char*)errorsCS->GetBufferPointer());
    }

    // Query shaders to properly build root signature

    if (vsResult == S_OK) {
        _queryShaderResources(compiledVS);
    }
    if (psResult == S_OK) {
        _queryShaderResources(compiledPS);
    }
    if (csResult == S_OK) {
        _queryShaderResources(compiledCS);
    }

    CD3DX12_ROOT_PARAMETER* RP = new CD3DX12_ROOT_PARAMETER[_resourceDescriptorTable.size()];
    CD3DX12_DESCRIPTOR_RANGE* srvTableRange = nullptr; //reuse
    CD3DX12_DESCRIPTOR_RANGE* samplerTableRange = nullptr; //reuse

    int i = 0;
    int rootParameterIndex = 0;
    for (auto resource : _resourceDescriptorTable) {
        if (resource.second.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER) {

            UINT bytes = 0;
            for (auto shaderInput : _constBuffDescriptorTable[resource.second.Name]) {
                bytes = max(bytes, shaderInput.StartOffset + shaderInput.Size);
            }

            if (std::string(resource.second.Name).compare("objectData") == 0) { //use root constants for per model objects
                
                RP[resource.second.uID + rootParameterIndex].InitAsConstants(bytes / 4,
                    resource.second.uID + rootParameterIndex);
                _constantBuffers[resource.second.Name] = new ConstantBuffer(device, _constBuffDescriptorTable[resource.second.Name]);
                _resourceIndexes[resource.second.Name] = resource.second.uID + rootParameterIndex;
            }
            else {
                RP[resource.second.uID + rootParameterIndex].InitAsConstantBufferView(resource.second.uID + rootParameterIndex);
                _constantBuffers[resource.second.Name] = new ConstantBuffer(device, _constBuffDescriptorTable[resource.second.Name]);
                _resourceIndexes[resource.second.Name] = resource.second.uID + rootParameterIndex;
            }
            i++;
        }
    }
    rootParameterIndex = static_cast<int>(_resourceIndexes.size());
    for (auto resource : _resourceDescriptorTable) {
        if (resource.second.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_SAMPLER && csResult != S_OK) {
            samplerTableRange = new CD3DX12_DESCRIPTOR_RANGE();
            samplerTableRange->Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);
            RP[resource.second.uID + rootParameterIndex].InitAsDescriptorTable(1, samplerTableRange, D3D12_SHADER_VISIBILITY_PIXEL);
            _resourceIndexes[resource.second.Name] = resource.second.uID + rootParameterIndex;
            i++;
        }
    }
    rootParameterIndex = static_cast<int>(_resourceIndexes.size());
    for (auto resource : _resourceDescriptorTable) {
        if (resource.second.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE) {
            srvTableRange = new CD3DX12_DESCRIPTOR_RANGE();
            srvTableRange->Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, resource.second.uID);
            if (csResult == S_OK) {
                RP[resource.second.uID + rootParameterIndex].InitAsDescriptorTable(1, srvTableRange);
            }
            else {
                RP[resource.second.uID + rootParameterIndex].InitAsDescriptorTable(1, srvTableRange, D3D12_SHADER_VISIBILITY_PIXEL);
            }
            _resourceIndexes[resource.second.Name] = resource.second.uID + rootParameterIndex;
            i++;
        }
    }
    rootParameterIndex = static_cast<int>(_resourceIndexes.size());
    for (auto resource : _resourceDescriptorTable) {
        if (resource.second.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWTYPED) {
            srvTableRange = new CD3DX12_DESCRIPTOR_RANGE();
            srvTableRange->Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, resource.second.uID);
            RP[resource.second.uID + rootParameterIndex].InitAsDescriptorTable(1, srvTableRange);
            _resourceIndexes[resource.second.Name] = resource.second.uID + rootParameterIndex;
            i++;
        }
    }

    ComPtr<ID3DBlob> pOutBlob, pErrorBlob;
    CD3DX12_ROOT_SIGNATURE_DESC descRootSignature;
    D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
    if (csResult == S_OK) {

        bool foundStaticSampler = false;
        for (auto resource : _resourceDescriptorTable) {
            if (resource.second.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_SAMPLER) {
                foundStaticSampler = true;
            }
        }

        if (foundStaticSampler) {
            
            samplerDesc.Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
            samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
            samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
            samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
            samplerDesc.MipLODBias = 0.0f;
            samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
            samplerDesc.MinLOD = 0.0f;
            samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
            samplerDesc.MaxAnisotropy = 0;
            samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
            samplerDesc.ShaderRegister = 0;
            samplerDesc.RegisterSpace = 0;
            samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

            descRootSignature.Init(static_cast<UINT>(_resourceIndexes.size()),
                &RP[0],
                1,
                &samplerDesc,
                D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
        }
        else {
            descRootSignature.Init(static_cast<UINT>(_resourceIndexes.size()),
                &RP[0],
                0,
                nullptr,
                D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
        }
    }
    else {
        descRootSignature.Init(static_cast<UINT>(_resourceIndexes.size()),
            &RP[0],
            0,
            nullptr,
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
    }

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

    if (csResult != S_OK) {
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
            "NORMAL",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            12,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
            });

        _inputLayout.push_back({
            "UV",
            0,
            DXGI_FORMAT_R32G32_FLOAT,
            0,
            24,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
            });
        D3D12_GRAPHICS_PIPELINE_STATE_DESC pso;
        ZeroMemory(&pso, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
        pso.InputLayout = { _inputLayout.data(), static_cast<UINT>(_inputLayout.size()) };
        pso.pRootSignature = _rootSignature.Get();
        if (vsResult == S_OK) {
            pso.VS = {
                reinterpret_cast<BYTE*>(compiledVS->GetBufferPointer()),
                compiledVS->GetBufferSize()
            };
        }
        if (psResult == S_OK) {
            pso.PS = {
                reinterpret_cast<BYTE*>(compiledPS->GetBufferPointer()),
                compiledPS->GetBufferSize()
            };
        }
        auto rasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        rasterizerState.CullMode = D3D12_CULL_MODE_NONE; //disabled culling 
        pso.RasterizerState = rasterizerState;
        pso.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        pso.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        pso.SampleMask = UINT_MAX;
        pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

        bool foundDepthStencil = false;
        if (rtvs != nullptr) {
            int j = 0;
            for (auto rtv : *rtvs) {
                if (rtv == DXGI_FORMAT_D32_FLOAT || rtv == DXGI_FORMAT_R32_TYPELESS) {
                    foundDepthStencil = true;
                }
                else {
                    pso.RTVFormats[j++] = rtv;
                    pso.NumRenderTargets++;
                }
            }
        }
        else {
            pso.NumRenderTargets = 0;
        }

        pso.SampleDesc.Quality = 0;
        pso.SampleDesc.Count = 1;

        if (foundDepthStencil) {
            pso.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        }
        else {
            pso.DSVFormat = DXGI_FORMAT_UNKNOWN;
        }

        HRESULT result = device->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&_psoState));
    }
    else {
        D3D12_COMPUTE_PIPELINE_STATE_DESC pso;
        ZeroMemory(&pso, sizeof(D3D12_COMPUTE_PIPELINE_STATE_DESC));
        pso.pRootSignature = _rootSignature.Get();
        if (csResult == S_OK) {
            pso.CS = {
                reinterpret_cast<BYTE*>(compiledCS->GetBufferPointer()),
                compiledCS->GetBufferSize()
            };
        }
        pso.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
        HRESULT result = device->CreateComputePipelineState(&pso, IID_PPV_ARGS(&_psoState));
    }

}

void HLSLShader::setOM(std::vector<RenderTexture> targets, int width, int height) {
    auto device = DXLayer::instance()->getDevice();
    auto cmdList = DXLayer::instance()->getCmdList();

    D3D12_VIEWPORT viewPort =
    {
        0.0f,
        0.0f,
        static_cast<float>(width),
        static_cast<float>(height),
        0.0f,
        1.0f
    };

    // Scissor rectangle

    D3D12_RECT rectScissor = { 0, 0, (LONG)width,(LONG)height};

    D3D12_CPU_DESCRIPTOR_HANDLE* handles = new D3D12_CPU_DESCRIPTOR_HANDLE[targets.size() - 1];
    int handleIndex = 0;
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle;
    bool containsDepthStencil = false;
    UINT rtvCount = 0;
    for (auto buffer : targets) {
        if (buffer.getFormat() == DXGI_FORMAT_D32_FLOAT || buffer.getFormat() == DXGI_FORMAT_R32_TYPELESS) {
            dsvHandle = buffer.getHandle();
            buffer.bindTarget(D3D12_RESOURCE_STATE_DEPTH_WRITE);
            containsDepthStencil = true;
        }
        else {
            handles[handleIndex++] = buffer.getHandle();
            buffer.bindTarget(D3D12_RESOURCE_STATE_RENDER_TARGET);
            rtvCount++;
        }
    }

    const float clear[] = { 0.0f, 0.0f, 0.0f, 1.000f };

    cmdList->RSSetViewports(1, &viewPort);
    cmdList->RSSetScissorRects(1, &rectScissor);

    if (containsDepthStencil) {
        cmdList->OMSetRenderTargets(
            rtvCount,
            handles,
            false,
            &dsvHandle);
    }
    else {
        cmdList->OMSetRenderTargets(
            rtvCount,
            handles,
            false,
            nullptr);
    }

    int rtvIndex = 0;
    for (int i = 0; i < targets.size(); i++) {

        // Clear target
        if (targets[i].getFormat() == DXGI_FORMAT_D32_FLOAT || targets[i].getFormat() == DXGI_FORMAT_R32_TYPELESS) {

            cmdList->ClearDepthStencilView(
                dsvHandle,
                D3D12_CLEAR_FLAG_DEPTH,
                1.0f,
                0,
                NULL,
                0
            );
        }
        else {

            cmdList->ClearRenderTargetView(
                handles[rtvIndex++],
                clear,
                NULL,
                0
            );
        }
    }
}

void HLSLShader::releaseOM(std::vector<RenderTexture> targets) {
    for (auto buffer : targets) {
        if (buffer.getFormat() == DXGI_FORMAT_D32_FLOAT || buffer.getFormat() == DXGI_FORMAT_R32_TYPELESS) {
            buffer.unbindTarget(D3D12_RESOURCE_STATE_DEPTH_WRITE);
        }
        else {
            buffer.unbindTarget(D3D12_RESOURCE_STATE_RENDER_TARGET);
        }
    }
}


void HLSLShader::_queryShaderResources(ComPtr<ID3DBlob> shaderBlob) {

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
            auto constBuff = reflectionInterface->GetConstantBufferByName(resourceDesc.Name);
            if (constBuff != nullptr) {
                ID3D12ShaderReflectionVariable* constBufferVar = nullptr;
                for (int j = 0; ; j++) {
                    constBufferVar = constBuff->GetVariableByIndex(j);
                    D3D12_SHADER_VARIABLE_DESC* ref = new D3D12_SHADER_VARIABLE_DESC();
                    auto isVariable = constBufferVar->GetDesc(ref);
                    if (isVariable != S_OK) {
                        break;
                    }
                    else {
                        bool found = false;
                        for (auto subIndex : _constBuffDescriptorTable[resourceDesc.Name]) {
                            if (std::string(subIndex.Name).compare(std::string(ref->Name)) == 0) {
                                found = true;
                                break;
                            }
                        }
                        if (!found) {
                            _constBuffDescriptorTable[resourceDesc.Name].push_back(*ref);
                        }
                    }
                }
            }
            _resourceDescriptorTable[resourceDesc.Name] = resourceDesc;
        }
    }
}

void HLSLShader::updateData(std::string id, void* data) {
    
    auto cmdList = DXLayer::instance()->getCmdList();
    for (auto constBuffEntry : _constBuffDescriptorTable) {
        for (auto entry : constBuffEntry.second) {
            std::string identifier = id;
            size_t index = identifier.find("[");
            if (index != std::string::npos) {
                identifier = identifier.substr(0, index);
            }
            if (std::string(entry.Name).compare(identifier) == 0) {
                _constantBuffers[constBuffEntry.first]->update(cmdList, data, constBuffEntry.first,
                    _resourceIndexes[constBuffEntry.first], entry.Size, entry.StartOffset);
            }
        }
    }
}

void HLSLShader::updateData(std::string dataName,
    int textureUnit,
    Texture* texture) {

    auto cmdList = DXLayer::instance()->getCmdList();
    texture->bindToDXShader(cmdList, _resourceIndexes[dataName], _resourceIndexes);
}

void HLSLShader::updateData(std::string id,
    UINT textureUnit,
    Texture* texture,
    ImageData imageInfo) {

    auto cmdList = DXLayer::instance()->getCmdList();
    texture->bindToDXShader(cmdList, _resourceIndexes[id], _resourceIndexes);
}


void HLSLShader::bind() {
    auto cmdList = DXLayer::instance()->getCmdList();
    cmdList->SetPipelineState(_psoState.Get());
    if (_inputLayout.size() > 0) {
        cmdList->SetGraphicsRootSignature(_rootSignature.Get());
    }
    else {
        cmdList->SetComputeRootSignature(_rootSignature.Get());
    }
}

void HLSLShader::unbind() {
}

void HLSLShader::bindAttributes(VAO* vao) {
    auto cmdList = DXLayer::instance()->getCmdList();

    if (vao != nullptr) {
        cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmdList->IASetIndexBuffer(&(vao->getIndexBuffer()));
        D3D12_VERTEX_BUFFER_VIEW vertexBuffers[] = { vao->getVertexBuffer() };
        cmdList->IASetVertexBuffers(0, 1, vertexBuffers);
    }
    else {

        cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        cmdList->IASetIndexBuffer(&_ibv);
    }
}

void HLSLShader::unbindAttributes() {
}

void HLSLShader::dispatch(int x, int y, int z) {

    DXLayer::instance()->getCmdList()->Dispatch(x, y, z);
}

void HLSLShader::draw(int offset, int instances, int numTriangles) {

    DXLayer::instance()->getCmdList()->DrawIndexedInstanced(numTriangles, 1, offset, 0, 0);
}
