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
    
    ~DXLayer();

    static DXLayer*                   instance();
    static void                       initialize(HINSTANCE hInstance, DWORD width, DWORD height, int cmdShow);
    void                              initCmdLists();

    void                              flushCommandList();
    void                              present(Texture* renderTexture);

    ComPtr<ID3D12GraphicsCommandList> getCmdList();
    ComPtr<ID3D12Device>              getDevice();

private:
    DXLayer(HINSTANCE hInstance, DWORD width, DWORD height, int cmdShow);

    PresentTarget*                    _presentTarget;
    ComPtr<ID3D12Device>              _device;
    ComPtr<ID3D12CommandAllocator>    _cmdAllocator;
    ComPtr<ID3D12GraphicsCommandList> _cmdLists[CMD_LIST_NUM];
    ComPtr<ID3D12Fence>               _cmdListFence;
    ComPtr<ID3D12CommandQueue>        _cmdQueue;
    HWND                              _window;
    HANDLE                            _event;
    int                               _cmdShow;
    int                               _cmdListIndex;
    int                               _nextFenceValue;
    int                               _cmdListFenceValues[CMD_LIST_NUM];

    static DXLayer*                   _dxLayer;
    const DXGI_FORMAT                 _rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

};