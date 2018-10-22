#include "DXLayer.h"
#include "ModelBroker.h"
#include "ShaderBroker.h"
#include "HLSLShader.h"
#include "Light.h"

DXLayer* DXLayer::_dxLayer = nullptr;

// init window class
// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    // sort through and find what code to run for the message given
    switch (message)
    {
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    } break;
    case WM_SIZE:
    {
        //g_windowWidth = 0xFFFF & lParam;
        //g_windowHeight = 0xFFFF & (lParam >> 16);

        // If maximizing the window or restoring from the maximized state, a WM_EXITSIZEMOVE
        // message won't be triggered, so react to the new window size here instead
        switch (wParam)
        {
        case SIZE_RESTORED:
            break;
        case SIZE_MAXIMIZED:
            break;
        }
        return 0;
    } break;
    case WM_EXITSIZEMOVE:
    {
        return 0;
    } break;
    }

    // Handle any messages the switch statement didn't
    return DefWindowProc(hWnd, message, wParam, lParam);
}

DXLayer::DXLayer(HINSTANCE hInstance, DWORD width, DWORD height, int cmdShow) :
    _cmdShow(cmdShow),
    _cmdListIndex(0) {
    WNDCLASSEX windowClass;

    ZeroMemory(&windowClass, sizeof(WNDCLASSEX));
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.lpszClassName = "DX12_HELLO_WORLD";
    RegisterClassEx(&windowClass);

    RECT windowRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);    // adjust the size

                                                                  // create the window and store a handle to it

    _window = CreateWindowEx(NULL,
        windowClass.lpszClassName,          // name of the window class
        windowClass.lpszClassName,
        WS_OVERLAPPEDWINDOW,
        300,
        300,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        NULL,       // we have no parent window, NULL
        NULL,       // we aren't using menus, NULL
        hInstance,  // application handle
        NULL);      // used with multiple windows, NULL



    _event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    // Device

#ifdef _DEBUG
    ComPtr<ID3D12Debug> debug;
    D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()));
    debug->EnableDebugLayer();
#endif

    D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(_device.GetAddressOf()));

    // Command allocator

    _device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(_cmdAllocator.GetAddressOf()));

    // Command queue

    D3D12_COMMAND_QUEUE_DESC cqDesc;
    ZeroMemory(&cqDesc, sizeof(cqDesc));
    _device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(_cmdQueue.GetAddressOf()));

    // Command lists

    for (int i = 0; i < NUM_SWAP_CHAIN_BUFFERS; i++) {
        _device->CreateCommandList(0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            _cmdAllocator.Get(),
            nullptr,
            IID_PPV_ARGS(_cmdLists[i].GetAddressOf()));

        _cmdLists[i]->Close();
    }


    _cmdAllocator->Reset();
    _cmdLists[_cmdListIndex]->Reset(_cmdAllocator.Get(), nullptr);

    // Fence

    _device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_cmdListFence.GetAddressOf()));

    //The GPU timestamp counter frequency (in ticks/second).
    UINT64 timestamp;
    auto result = _cmdQueue->GetTimestampFrequency(&timestamp);

    printf("%i\n", result);

    _presentTarget = new PresentTarget(_device, _rtvFormat, _cmdQueue, height, width, _window);


}

void DXLayer::initialize(HINSTANCE hInstance, DWORD width, DWORD height, int cmdShow) {
    
    if (_dxLayer == nullptr) {
        _dxLayer = new DXLayer(hInstance, width, height, cmdShow);
    }
}

DXLayer* DXLayer::instance() {
    return _dxLayer;
}

DXLayer::~DXLayer() {

}

void DXLayer::run(DeferredRenderer* deferred, 
    std::vector<Entity*> entities, 
    std::vector<Light*> lightList,
    ViewEventDistributor* viewEventDistributor) {

    _deferredFBO = new DeferredFrameBuffer();

    // show the window
    ShowWindow(_window, _cmdShow);

    // this struct holds Windows event messages
    MSG msg = { 0 };

    DWORD dwLastTickCount = GetTickCount();

    // main loop
    while (TRUE) {
        // check to see if any messages are waiting in the queue
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            // translate keystroke messages into the right format
            TranslateMessage(&msg);

            // send the message to the WindowProc function
            DispatchMessage(&msg);

            // check to see if it's time to quit
            if (msg.message == WM_QUIT)
                break;
        }
        else {
            _render(deferred, entities, lightList, viewEventDistributor);
        }
    }
}

void DXLayer::_render(DeferredRenderer* deferred, 
    std::vector<Entity*> entities, 
    std::vector<Light*> lightList,
    ViewEventDistributor* viewEventDistributor) {

    // Open command list
    _cmdAllocator->Reset();
    _cmdLists[_cmdListIndex]->Reset(_cmdAllocator.Get(), nullptr);

    //send all vbo data to point light shadow pre pass
    for (Light* light : lightList) {
        light->renderShadow(entities);
    }

    deferred->bind();

    _staticShader = static_cast<StaticShader*>(ShaderBroker::instance()->getShader("staticShader"));

    for (auto entity : entities) {
        
        _staticShader->runShader(entity);
    }

    deferred->unbind();

    RenderTexture* renderTexture = static_cast<RenderTexture*>(_deferredFBO->getRenderTexture());
    RenderTexture* depthTexture = static_cast<RenderTexture*>(_deferredFBO->getDepthTexture());

    std::vector<RenderTexture> textures = { *renderTexture, *depthTexture };
    HLSLShader::setOM(textures,
        IOEventDistributor::screenPixelWidth, IOEventDistributor::screenPixelHeight);
    
    //Pass lights to deferred shading pass
    deferred->deferredLighting(lightList, viewEventDistributor, nullptr, nullptr);

    HLSLShader::releaseOM(textures);

    auto presentShader = static_cast<MergeShader*>(ShaderBroker::instance()->getShader("mergeShader"));
    
    _presentTarget->bindTarget(_device, _cmdLists[_cmdListIndex], _cmdListIndex);
    
    auto texture = _deferredFBO->getRenderTexture();
    presentShader->runShader(texture, nullptr);

    _presentTarget->unbindTarget(_cmdLists[_cmdListIndex], _cmdListIndex);

    flushCommandList();
}

int DXLayer::getCmdListIndex() {
    return _cmdListIndex;
}

ComPtr<ID3D12CommandQueue> DXLayer::getCmdQueue() {
    return _cmdQueue;
}

ComPtr<ID3D12CommandAllocator> DXLayer::getCmdAllocator() {
    return _cmdAllocator;
}

void DXLayer::flushCommandList() {
   
    // Submit the current command list
    _cmdLists[_cmdListIndex]->Close();
    _cmdQueue->ExecuteCommandLists(1, CommandListCast(_cmdLists[_cmdListIndex].GetAddressOf()));

    _presentTarget->present();

    _cmdQueue->Signal(_cmdListFence.Get(), _nextFenceValue);
    _cmdListFence->GetCompletedValue();
    _cmdListFenceValues[_cmdListIndex] = _nextFenceValue++;

    // Wait for just-submitted command list to finish
    _cmdListFence->SetEventOnCompletion(_cmdListFenceValues[_cmdListIndex], _event);
    WaitForSingleObject(_event, INFINITE);
    
    _cmdListIndex = ++_cmdListIndex % NUM_SWAP_CHAIN_BUFFERS;
}

ComPtr<ID3D12GraphicsCommandList> DXLayer::getCmdList() {
    return _cmdLists[_cmdListIndex];
}
ComPtr<ID3D12Device> DXLayer::getDevice() {
    return _device;
}
PresentTarget* DXLayer::getPresentTarget() {
    return _presentTarget;
}