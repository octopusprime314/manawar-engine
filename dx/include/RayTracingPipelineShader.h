#pragma once
#include <string>
#include <wrl.h>
#include "d3d12_1.h"
#include "d3d12.h"
#include "d3dx12.h"
#include <vector>
#include <D3Dcompiler.h>
#include <map>
#include "PipelineShader.h"
#include "dxc/dxcapi.h"
#include "dxc/dxcapi.use.h"
#include "VAO.h"

using namespace Microsoft::WRL;

namespace GlobalRootSignatureParams {
    enum Value {
        OutputViewSlot = 0,
        AccelerationStructureSlot,
        SceneConstantSlot,
        VertexBuffersSlot,
        Count
    };
}

namespace LocalRootSignatureParams {
    enum Value {
        CubeConstantSlot = 0,
        Count
    };
}

struct Viewport
{
    float left;
    float top;
    float right;
    float bottom;
};

struct RayGenConstantBuffer
{
    Viewport viewport;
    Viewport stencil;
};

#define SizeOfInUint32(obj) ((sizeof(obj) - 1) / sizeof(UINT32) + 1)

class RayTracingPipelineShader : public PipelineShader {
    
    virtual void                       _queryShaderResources(ComPtr<ID3DBlob> shaderBlob);
    UINT                               _createBufferSRV(VAO* buffer,
                                        UINT numElements, UINT elementSize);
    UINT                               _allocateDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE* cpuDescriptor, 
                                        UINT descriptorIndexToUse = UINT_MAX);
    // DirectX Raytracing (DXR) attributes
    ComPtr<ID3D12Device5>              _dxrDevice;
    ComPtr<ID3D12GraphicsCommandList4> _dxrCommandList;
    ComPtr<ID3D12StateObjectPrototype> _dxrStateObject;

    // Root signatures
    ComPtr<ID3D12RootSignature>        _raytracingGlobalRootSignature;
    ComPtr<ID3D12RootSignature>        _raytracingLocalRootSignature;

    // Raytracing scene
    RayGenConstantBuffer               _cubeCB;

    ComPtr<ID3D12DescriptorHeap>       _descriptorHeap;
    UINT                               _descriptorSize;
    UINT                               _descriptorsAllocated;

    dxc::DxcDllSupport                 _dllSupport;

public:
    RayTracingPipelineShader(std::string shader,
                   ComPtr<ID3D12Device> device,
                   DXGI_FORMAT format);

};