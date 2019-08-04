#pragma once

#include <d3d12.h>
#include "d3dx12.h"
#include <D3Dcompiler.h>
#include <wrl.h>
#include <DirectXMath.h>
#include "DXGI.h"
#include "D3d12SDKLayers.h"
#include <vector>
#include "ConstantBuffer.h"
#include "ResourceBuffer.h"
#include "PipelineShader.h"
#include "PresentTarget.h"
#include "StaticShader.h"
#include "MergeShader.h"
#include "Entity.h"
#include "DeferredRenderer.h"
#include "DeferredFrameBuffer.h"
#include "ForwardRenderer.h"
#include "RayTracingPipelineShader.h"

#pragma comment(lib, "D3D12.lib") 
#pragma comment(lib, "dxgi.lib") 
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib") 

// These settings can be adjusted as needed
#define MAX_SRVS 128
#define CMD_LIST_NUM 2
using namespace Microsoft::WRL;


class DXLayer {
public:

    static void                       initialize(HINSTANCE hInstance, int cmdShow);
    void                              present(Texture* renderTexture);
    bool                              supportsRayTracing();
    void                              flushCommandList();
    void                              fenceCommandList();
    ComPtr<ID3D12CommandAllocator>    getCmdAllocator();
    UINT                              getCmdListIndex();
    void                              initCmdLists();
    ComPtr<ID3D12CommandQueue>        getCmdQueue();
    ComPtr<ID3D12GraphicsCommandList> getCmdList();
    ComPtr<ID3D12Device>              getDevice();
    static DXLayer*                   instance();

private:
    DXLayer(HINSTANCE hInstance,
            int cmdShow);
    ~DXLayer();

    const DXGI_FORMAT                 _rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    int                               _cmdListFenceValues[CMD_LIST_NUM];
    ComPtr<ID3D12GraphicsCommandList> _cmdLists[CMD_LIST_NUM];
    bool                              _rayTracingEnabled;
    int                               _nextFenceValue;
    PresentTarget*                    _presentTarget;
    ComPtr<ID3D12CommandAllocator>    _cmdAllocator;
    ComPtr<ID3D12Fence>               _cmdListFence;
    int                               _cmdListIndex;
    ComPtr<ID3D12CommandQueue>        _cmdQueue;
    int                               _cmdShow;
    static DXLayer*                   _dxLayer;
    ComPtr<ID3D12Device>              _device;
    HWND                              _window;
    HANDLE                            _event;
};