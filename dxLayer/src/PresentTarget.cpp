#include "PresentTarget.h"

PresentTarget::PresentTarget(ComPtr<ID3D12Device> device,
                             DXGI_FORMAT format,
                             ComPtr<ID3D12CommandQueue> cmdQueue,
                             int width,
                             int height,
                             HWND window) {

    // Descriptor heaps
    D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc;
    ZeroMemory(&descHeapDesc, sizeof(descHeapDesc));
    descHeapDesc.NumDescriptors = 2;
    descHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    descHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    device->CreateDescriptorHeap(&descHeapDesc,
                                 IID_PPV_ARGS(_rtvDescriptorHeap.GetAddressOf()));

    descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    device->CreateDescriptorHeap(&descHeapDesc,
                                 IID_PPV_ARGS(_dsvDescriptorHeap.GetAddressOf()));
    

    // Swap Chain
    DXGI_SWAP_CHAIN_DESC descSwapChain;
    ZeroMemory(&descSwapChain, sizeof(descSwapChain));
    descSwapChain.BufferCount        = NUM_SWAP_CHAIN_BUFFERS;
    descSwapChain.BufferDesc.Format  = format;
    descSwapChain.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    descSwapChain.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    descSwapChain.OutputWindow       = window;
    descSwapChain.SampleDesc.Count   = 1;
    descSwapChain.SampleDesc.Quality = 0;
    descSwapChain.Windowed           = TRUE;

    ComPtr<IDXGIFactory> pDxgiFactory;
    CreateDXGIFactory1(IID_PPV_ARGS(&pDxgiFactory));

    pDxgiFactory->CreateSwapChain(cmdQueue.Get(),
                                  &descSwapChain,
                                 _swapChain.GetAddressOf());

    // Backbuffer / render target
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    auto rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    _swapChain->GetBuffer(0, IID_PPV_ARGS(_backBuffers[0].GetAddressOf()));
    device->CreateRenderTargetView(_backBuffers[0].Get(),
                                   nullptr,
                                   rtvHandle);

    rtvHandle.Offset(rtvDescriptorSize);
    _swapChain->GetBuffer(         1,
                                   IID_PPV_ARGS(_backBuffers[1].GetAddressOf()));

    device->CreateRenderTargetView(_backBuffers[1].Get(),
                                   nullptr,
                                   rtvHandle);

    // Depth Buffer
    D3D12_CLEAR_VALUE depthOptimizedClearValue;
    ZeroMemory(&depthOptimizedClearValue, sizeof(depthOptimizedClearValue));
    depthOptimizedClearValue.Format               = DXGI_FORMAT_D32_FLOAT;
    depthOptimizedClearValue.DepthStencil.Depth   = 1.0f;
    depthOptimizedClearValue.DepthStencil.Stencil = 0;

    device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                                    D3D12_HEAP_FLAG_NONE,
                                    &CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT,
                                                                  width,
                                                                  height,
                                                                  1,
                                                                  1,
                                                                  1,
                                                                  0,
                                                                  D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
                                    D3D12_RESOURCE_STATE_DEPTH_WRITE,
                                    &depthOptimizedClearValue,
                                    IID_PPV_ARGS(_depthBuffer.GetAddressOf()));

    D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    depthStencilViewDesc.Format        = DXGI_FORMAT_D32_FLOAT;
    depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
    depthStencilViewDesc.Flags         = D3D12_DSV_FLAG_NONE;

    // Backbuffer / render target
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    auto dsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

    device->CreateDepthStencilView(_depthBuffer.Get(),
                                   &depthStencilViewDesc, dsvHandle);

    // Viewport
    _viewPort =
    {
        0.0f,
        0.0f,
        static_cast<float>(width),
        static_cast<float>(height),
        0.0f,
        1.0f
    };

    // Scissor rectangle

    _rectScissor = 
    { 
        0,
        0,
        static_cast<LONG>(width),
        static_cast<LONG>(height)
    };
}

void PresentTarget::bindTarget(ComPtr<ID3D12Device> device, 
                               ComPtr<ID3D12GraphicsCommandList> cmdList, 
                               int swapchainIndex) {

    D3D12_RESOURCE_BARRIER          barrierDesc;
    ZeroMemory(&barrierDesc, sizeof(barrierDesc));

    barrierDesc.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrierDesc.Transition.pResource   = _backBuffers[swapchainIndex].Get();
    barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrierDesc.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
    const float midnightBlue[]         = { 0.098f, 0.098f, 0.439f, 1.000f };
    auto rtvDescriptorSize             = device->GetDescriptorHandleIncrementSize(
                                                        D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    cmdList->ResourceBarrier(  1, &barrierDesc);
    cmdList->RSSetViewports(   1, &_viewPort);
    cmdList->RSSetScissorRects(1, &_rectScissor);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    rtvHandle.Offset(rtvDescriptorSize * swapchainIndex);
    cmdList->OMSetRenderTargets(   1,
                                   &rtvHandle,
                                   true,
                                   &_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    // Clear targets
    cmdList->ClearRenderTargetView(rtvHandle,
                                   midnightBlue,
                                   NULL,
                                   0);

    cmdList->ClearDepthStencilView(_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
                                   D3D12_CLEAR_FLAG_DEPTH,
                                   1.0f,
                                   0,
                                   NULL,
                                   0);
}

void PresentTarget::unbindTarget(ComPtr<ID3D12GraphicsCommandList> cmdList,
                                 int swapchainIndex) {
    
    D3D12_RESOURCE_BARRIER          barrierDesc;
    ZeroMemory(&barrierDesc, sizeof(barrierDesc));

    barrierDesc.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrierDesc.Transition.pResource   = _backBuffers[swapchainIndex].Get();
    barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrierDesc.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;

    cmdList->ResourceBarrier(1, &barrierDesc);
}

void PresentTarget::present() {
    _swapChain->Present(0, 0);
}
