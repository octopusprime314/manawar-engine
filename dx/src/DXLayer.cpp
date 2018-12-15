#include "DXLayer.h"
#include "ModelBroker.h"
#include "ShaderBroker.h"
#include "HLSLShader.h"
#include "Light.h"
#include "IOEventDistributor.h"

DXLayer* DXLayer::_dxLayer = nullptr;

DXLayer::DXLayer(HINSTANCE hInstance, DWORD width, DWORD height, int cmdShow) :
    _cmdShow(cmdShow),
    _cmdListIndex(0) {
    WNDCLASSEX windowClass;

    _nextFenceValue = 1;
    _cmdListFenceValues[0] = _cmdListFenceValues[1] = 0;

    ZeroMemory(&windowClass, sizeof(WNDCLASSEX));
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = IOEventDistributor::dxEventLoop;
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
        0,
        0,
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
    _cmdLists[0]->Reset(_cmdAllocator.Get(), nullptr);
    _cmdLists[0]->Close();
    _cmdAllocator->Reset();
    _cmdLists[1]->Reset(_cmdAllocator.Get(), nullptr);
    _cmdLists[1]->Close();

    _cmdAllocator->Reset();
    _cmdLists[0]->Reset(_cmdAllocator.Get(), nullptr);
    // Fence

    _device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_cmdListFence.GetAddressOf()));

    //The GPU timestamp counter frequency (in ticks/second).
    UINT64 timestamp;
    auto result = _cmdQueue->GetTimestampFrequency(&timestamp);

    printf("%i\n", result);

    _presentTarget = new PresentTarget(_device, _rtvFormat, _cmdQueue, height, width, _window);

    // show the window
    ShowWindow(_window, _cmdShow);
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

void DXLayer::initCmdLists() {
    // Open command list
    _cmdAllocator->Reset();
    _cmdLists[_cmdListIndex]->Reset(_cmdAllocator.Get(), nullptr);
}

void DXLayer::present(Texture* renderTexture) {
    auto presentShader = static_cast<MergeShader*>(ShaderBroker::instance()->getShader("mergeShader"));

    _presentTarget->bindTarget(_device, _cmdLists[_cmdListIndex], _cmdListIndex);

    presentShader->runShader(renderTexture, nullptr);

    _presentTarget->unbindTarget(_cmdLists[_cmdListIndex], _cmdListIndex);

    flushCommandList();
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

void DXLayer::fenceCommandList() {

    // Submit the current command list
    _cmdLists[_cmdListIndex]->Close();
    _cmdQueue->ExecuteCommandLists(1, CommandListCast(_cmdLists[_cmdListIndex].GetAddressOf()));

    _cmdQueue->Signal(_cmdListFence.Get(), _nextFenceValue);
    _cmdListFence->GetCompletedValue();
    _cmdListFenceValues[_cmdListIndex] = _nextFenceValue++;

    // Wait for just-submitted command list to finish
    _cmdListFence->SetEventOnCompletion(_cmdListFenceValues[_cmdListIndex], _event);
    WaitForSingleObject(_event, INFINITE);
}

ComPtr<ID3D12GraphicsCommandList> DXLayer::getCmdList() {
    return _cmdLists[_cmdListIndex];
}
ComPtr<ID3D12Device> DXLayer::getDevice() {
    return _device;
}
ComPtr<ID3D12CommandAllocator> DXLayer::getCmdAllocator() {
    return _cmdAllocator;
}
ComPtr<ID3D12CommandQueue> DXLayer::getCmdQueue() {
    return _cmdQueue;
}
UINT DXLayer::getCmdListIndex() {
    return _cmdListIndex;
}
