#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "d3dx12.h"
#include "DXGI.h"
#include "RenderTexture.h"

#define NUM_SWAP_CHAIN_BUFFERS 2

using namespace Microsoft::WRL;

class PresentTarget {

    ComPtr<ID3D12DescriptorHeap> _rtvDescriptorHeap;
    ComPtr<ID3D12DescriptorHeap> _dsvDescriptorHeap;
    ComPtr<IDXGISwapChain>       _swapChain;
    ComPtr<ID3D12Resource>       _backBuffers[NUM_SWAP_CHAIN_BUFFERS];
    ComPtr<ID3D12Resource>       _depthBuffer;
    D3D12_VIEWPORT               _viewPort;
    D3D12_RECT                   _rectScissor;

public:
    PresentTarget(ComPtr<ID3D12Device> device,
                  DXGI_FORMAT format,
                  ComPtr<ID3D12CommandQueue> cmdQueue,
                  int width, int height, HWND window);

    void bindTarget(ComPtr<ID3D12Device> device,
                    ComPtr<ID3D12GraphicsCommandList> cmdList,
                    int swapchainIndex);

    void unbindTarget(ComPtr<ID3D12GraphicsCommandList> cmdList,
                      int swapchainIndex);

    void present();
};