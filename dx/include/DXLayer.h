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

    void                              flushCommandList();
    void                              run(DeferredRenderer* deffered, 
                                          std::vector<Entity*> entities,
                                          std::vector<Light*> lightList,
                                          ViewEventDistributor* viewEventDistributor,
                                          ForwardRenderer* forwardRenderer,
                                          SSAO* ssaoPass);
    static DXLayer*                   instance();
    static void                       initialize(HINSTANCE hInstance, DWORD width, DWORD height, int cmdShow);

    ComPtr<ID3D12GraphicsCommandList> getCmdList();
    ComPtr<ID3D12Device>              getDevice();
    int                               getCmdListIndex();
    PresentTarget*                    getPresentTarget();
    ComPtr<ID3D12CommandQueue>        getCmdQueue();
    ComPtr<ID3D12CommandAllocator>    getCmdAllocator();
    void                              present(Texture* renderTexture);
    void                              initCmdLists();

private:
    DXLayer(HINSTANCE hInstance, DWORD width, DWORD height, int cmdShow);
    void                              _render(DeferredRenderer* deferred,
                                              std::vector<Entity*> entities,
                                              std::vector<Light*> lightList,
                                              ViewEventDistributor* viewEventDistributor,
                                              ForwardRenderer* forwardRenderer,
                                              SSAO* ssaoPass);

    PresentTarget*                    _presentTarget;
    StaticShader*                     _staticShader;
    static DXLayer*                   _dxLayer;
    DeferredFrameBuffer*              _deferredFBO;

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
    const DXGI_FORMAT                 _rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

};