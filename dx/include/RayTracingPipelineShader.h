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
#include "RenderTexture.h"

using namespace Microsoft::WRL;

namespace GlobalRootSignatureParams {
    enum Value {
        OutputViewSlot = 0,
        AccelerationStructureSlot,
        SceneConstantSlot,
        IndexBuffersSlot,
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

struct Viewport {
    float left;
    float top;
    float right;
    float bottom;
};

// Geometry
struct D3DBuffer {
    ComPtr<ID3D12Resource> resource;
    D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle;
};

struct SceneConstantBuffer {
    float projectionToWorld[16];
    float cameraPosition[4];
    float lightPosition[4];
    float lightDirection[4];
    float projection[16];
    float lightView[16];
};

union AlignedSceneConstantBuffer {
    SceneConstantBuffer constants;
    uint8_t alignmentPadding[D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT];
};

struct GpuToCpuBuffers {
    ComPtr<ID3D12Resource> outputBuffer;
    ComPtr<ID3D12Resource> readbackBuffer;
    unsigned char*         cpuSideData;
};

#define SizeOfInUint32(obj) ((sizeof(obj) - 1) / sizeof(UINT32) + 1)

class RayTracingPipelineShader : public PipelineShader {
    
    virtual void                       _queryShaderResources(ComPtr<ID3DBlob> shaderBlob);
    UINT                               _allocateDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE* cpuDescriptor, 
                                                            UINT descriptorIndexToUse = UINT_MAX);
    UINT                               _createBufferSRV(D3DBuffer* buffer,
                                                    UINT numElements, UINT elementSize);
    void                               _populateDefaultHeap(GpuToCpuBuffers& resources, UINT64 byteSize = 8);
    void                               _gpuToCpuTransfer(GpuToCpuBuffers& resources, UINT64 byteSize = 8);
    void                               _readBackOnCpu(GpuToCpuBuffers& buffers, UINT64 byteSize = 8);
    // DirectX Raytracing (DXR) attributes
    ComPtr<ID3D12Device5>              _dxrDevice;
    ComPtr<ID3D12GraphicsCommandList4> _dxrCommandList;
    ComPtr<ID3D12StateObjectPrototype> _dxrStateObject;

    // Root signatures
    ComPtr<ID3D12RootSignature>        _raytracingGlobalRootSignature;
    ComPtr<ID3D12RootSignature>        _raytracingLocalRootSignature;

    // Raytracing scene
    SceneConstantBuffer                _sceneCB[2];
    ComPtr<ID3D12DescriptorHeap>       _descriptorHeap;
    UINT                               _descriptorSize;
    UINT                               _descriptorsAllocated;

    // Acceleration structure
    std::vector<ComPtr<ID3D12Resource>> _bottomLevelAccelerationStructure;
    ComPtr<ID3D12Resource>             _topLevelAccelerationStructure;
    ComPtr<ID3D12Resource>             _copiedBottomLevelAccelerationStructure;
    ComPtr<ID3D12Resource>             _copiedTopLevelAccelerationStructure;
    ComPtr<ID3D12Resource>             _copiedAS;
    AlignedSceneConstantBuffer*        _mappedConstantData;
    ComPtr<ID3D12Resource>             _perFrameConstants;
    dxc::DxcDllSupport                 _dllSupport;

    //Shader tables
    ComPtr<ID3D12Resource>             _hitGroupShaderTable;
    ComPtr<ID3D12Resource>             _missShaderTable;
    ComPtr<ID3D12Resource>             _rayGenShaderTable;

    // Raytracing output
    RenderTexture*                     _raytracingOutput;

    std::vector<D3DBuffer>             _indexBuffer;
    std::vector<D3DBuffer>             _vertexBuffer;

    ComPtr<ID3D12Resource>             _scratchResource;
    ComPtr<ID3D12Resource>             _instanceDescs;

    // Effort to fix hang
    D3D12_DISPATCH_RAYS_DESC           _dispatchDesc;
    D3D12_DESCRIPTOR_HEAP_DESC         _descriptorHeapDesc;
    D3D12_RAYTRACING_GEOMETRY_DESC*    _geometryDesc;
    D3D12_RAYTRACING_INSTANCE_DESC*    _instanceDesc;
    CD3DX12_CPU_DESCRIPTOR_HANDLE*     _hUAVDescriptor;
    D3D12_UNORDERED_ACCESS_VIEW_DESC   _uavDesc;
public:
    RayTracingPipelineShader(std::string shader,
                   ComPtr<ID3D12Device> device,
                   DXGI_FORMAT format, std::vector<Entity*>& entityList);

    void doRayTracing(Entity* entity, Light* light);

    RenderTexture* getRayTracingTarget();

};