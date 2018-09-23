#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "d3dx12.h"
#include "Matrix.h"
#include "PipelineShader.h"
using namespace Microsoft::WRL;

class ConstantBuffer {

public:
    ConstantBuffer(ComPtr<ID3D12Device> device);
    ~ConstantBuffer();
    void update(ComPtr<ID3D12GraphicsCommandList> cmdList,
                Matrix mvp,
                PipelineShader& pso);

private:
    ComPtr<ID3D12Resource>          _uploadBuffer;
    ComPtr<ID3D12DescriptorHeap>    _cbvHeap;
    D3D12_CONSTANT_BUFFER_VIEW_DESC _cbvDesc;
};