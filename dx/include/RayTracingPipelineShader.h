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
#include "Entity.h"

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

struct CubeConstantBuffer
{
    float albedo[4];
};

// Geometry
struct D3DBuffer
{
    ComPtr<ID3D12Resource> resource;
    D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle;
};

struct SceneConstantBuffer
{
    float projectionToWorld[16];
    float cameraPosition[4];
    float lightPosition[4];
    float lightAmbientColor[4];
    float lightDiffuseColor[4];
};

union AlignedSceneConstantBuffer
{
    SceneConstantBuffer constants;
    uint8_t alignmentPadding[D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT];
};

#define SizeOfInUint32(obj) ((sizeof(obj) - 1) / sizeof(UINT32) + 1)

class RayTracingPipelineShader : public PipelineShader {
    
    virtual void                       _queryShaderResources(ComPtr<ID3DBlob> shaderBlob);
    // DirectX Raytracing (DXR) attributes
    ComPtr<ID3D12Device5>              _dxrDevice;
    ComPtr<ID3D12GraphicsCommandList4> _dxrCommandList;
    ComPtr<ID3D12StateObjectPrototype> _dxrStateObject;

    // Root signatures
    ComPtr<ID3D12RootSignature>        _raytracingGlobalRootSignature;
    ComPtr<ID3D12RootSignature>        _raytracingLocalRootSignature;

    // Raytracing scene
    CubeConstantBuffer                 _cubeCB;
    SceneConstantBuffer                _sceneCB[2];
    ComPtr<ID3D12DescriptorHeap>       _descriptorHeap;
    UINT                               _descriptorSize;
    UINT                               _descriptorsAllocated;

    // Acceleration structure
    ComPtr<ID3D12Resource>             _bottomLevelAccelerationStructure;
    ComPtr<ID3D12Resource>             _topLevelAccelerationStructure;
    AlignedSceneConstantBuffer*        _mappedConstantData;
    ComPtr<ID3D12Resource>             _perFrameConstants;
    dxc::DxcDllSupport                 _dllSupport;

    //Shader tables
    ComPtr<ID3D12Resource>             _hitGroupShaderTable;
    ComPtr<ID3D12Resource>             _missShaderTable;
    ComPtr<ID3D12Resource>             _rayGenShaderTable;

    // Raytracing output
    ComPtr<ID3D12Resource>             _raytracingOutput;
    D3D12_GPU_DESCRIPTOR_HANDLE        _raytracingOutputResourceUAVGpuDescriptor;
    UINT                               _raytracingOutputResourceUAVDescriptorHeapIndex;

public:
    RayTracingPipelineShader(std::string shader,
                   ComPtr<ID3D12Device> device,
                   DXGI_FORMAT format, Entity* entity);

    void DoRayTracing(Entity* entity);

};