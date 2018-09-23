#include "DXLayer.h"
#include "TextureBroker.h"
#include "ModelBroker.h"
#include "VAO.h"

DXLayer::DXLayer(HWND wnd, DWORD width, DWORD height, int cmdShow) :
    _window(wnd),
    _cmdShow(cmdShow),
    _cmdListIndex(0) {

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

    TextureBroker::init(_cmdLists[0], _device);

    //Load and compile all shaders for the shader broker
     ModelBroker::instance()->buildModels();

    // Fence

   _device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_cmdListFence.GetAddressOf()));

    //The GPU timestamp counter frequency (in ticks/second).
    UINT64 timestamp;
    auto result = _cmdQueue->GetTimestampFrequency(&timestamp);

    printf("%i\n", result);

    _presentTarget = new PresentTarget(_device, _rtvFormat, _cmdQueue, height, width, wnd);

    _buildTriangle();

    _textureAsset = new AssetTexture("assets/textures/eyes/eyewerewolfbeastbrown.dds",
        _cmdLists[_cmdListIndex], _device);

    _flushCommandListAndWait(nullptr, true);

    //Constant buffers
    _mvpConstBuff = new ConstantBuffer(_device);

}

DXLayer::~DXLayer() {

}

void DXLayer::run() {

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
            _render();
        }
    }
}

void DXLayer::_render() {
   
    // Open command list
    _cmdAllocator->Reset();
    _cmdLists[_cmdListIndex]->Reset(_cmdAllocator.Get(), _pipelineShader->getPSO().Get());

    // Setup pipeline state / etc

    _cmdLists[_cmdListIndex]->SetGraphicsRootSignature(_pipelineShader->getRootSignature().Get());

    //_textureAsset->bindToShader(_cmdLists[_cmdListIndex], *_pipelineShader);

    Matrix p(Matrix::cameraProjection(45.0f, 1920.0f / 1080.0f, 0.1f, 1000.0f));
    Matrix v(Matrix::cameraTranslation(0.0f, 0.0f, 120.0f));

    _mvpConstBuff->update(_cmdLists[_cmdListIndex], p * v, *_pipelineShader);

    _cmdLists[_cmdListIndex]->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    _cmdLists[_cmdListIndex]->IASetIndexBuffer(&_ibv);
    D3D12_VERTEX_BUFFER_VIEW vertexBuffers[] = { _vbv };
    _cmdLists[_cmdListIndex]->IASetVertexBuffers(0, 1, vertexBuffers);


    _presentTarget->bindTarget(_device, _cmdLists[_cmdListIndex], _cmdListIndex);

    auto model = ModelBroker::instance()->getModel("werewolf/werewolf.fbx");
    auto textureStrides = (*model->getVAO())[0]->getTextureStrides();

    int indexLocation = 0;
    for (auto textureStride : textureStrides) {
        auto texture = TextureBroker::instance()->getTexture(textureStride.first);
        if (texture != nullptr) {
            texture->bindToShader(_cmdLists[_cmdListIndex], *_pipelineShader);
        }

        _cmdLists[_cmdListIndex]->DrawIndexedInstanced(textureStride.second, 1, indexLocation, 0, 0);
        indexLocation += textureStride.second;

        //_cmdLists[_cmdListIndex]->DrawIndexedInstanced(_ibv.SizeInBytes / sizeof(int16_t), 1, 0, 0, 0);
    }

    _presentTarget->unbindTarget(_cmdLists[_cmdListIndex], _cmdListIndex);

    _flushCommandListAndWait(nullptr, true);

}

void DXLayer::_flushCommandListAndWait(ID3D12PipelineState *pNextPSO, bool waitOnLatest) {
   
    // Submit the current command list
    _cmdLists[_cmdListIndex]->Close();
    _cmdQueue->ExecuteCommandLists(1, CommandListCast(_cmdLists[_cmdListIndex].GetAddressOf()));

    _presentTarget->present();

    _cmdQueue->Signal(_cmdListFence.Get(), _nextFenceValue);
    _cmdListFence->GetCompletedValue();
    _cmdListFenceValues[_cmdListIndex] = _nextFenceValue++;

    // Wait for just-submitted command list to finish
    if (waitOnLatest) {
        _cmdListFence->SetEventOnCompletion(_cmdListFenceValues[_cmdListIndex], _event);
        WaitForSingleObject(_event, INFINITE);
    }
    _cmdListIndex = ++_cmdListIndex % NUM_SWAP_CHAIN_BUFFERS;
}

void DXLayer::_buildTriangle() {
    
    _pipelineShader = new PipelineShader("color.hlsl", _device, _rtvFormat);

    auto model = ModelBroker::instance()->getModel("werewolf/werewolf.fbx");
    auto renderBuffers = model->getRenderBuffers();
    auto vertices = renderBuffers->getVertices();
    auto normals = renderBuffers->getNormals();
    auto textures = renderBuffers->getTextures();
    auto indices = renderBuffers->getIndices();

    //Now flatten vertices and normals out for opengl
    size_t triBuffSize = 0;
    float* flattenAttribs = nullptr; //Only include the x y and z values not w
    uint16_t* flattenIndexes = nullptr; //Only include the x y and z values not w, same size as vertices
   
    //Now flatten vertices and normals out for opengl
    triBuffSize = indices->size() * 3;
    flattenAttribs = new float[triBuffSize + (textures->size() * 2)]; //Only include the x y and z values not w
    flattenIndexes = new uint16_t[triBuffSize / 3]; //Only include the x y and z values not w, same size as vertices
    
    int i = 0; //iterates through vertices indexes
    uint16_t j = 0;
    for (auto index : *indices) {
        float *flatVert = (*vertices)[index].getFlatBuffer();
        flattenAttribs[i++] = flatVert[0];
        flattenAttribs[i++] = flatVert[1];
        flattenAttribs[i++] = -flatVert[2];
        flattenIndexes[j] = j;
        j++;
        i += 2;
    }

    i = 3; //Reset for texture indexes
    for (auto texture : *textures) {
        float *flat = texture.getFlatBuffer();
        flattenAttribs[i++] = flat[0];
        flattenAttribs[i++] = flat[1];
        i += 3;
    }

    struct Vertex {
        float pos[3];
        float uv[2];
    };

    //float vertexAttributes[] = { 
    //   -1.0f, -1.0f, -10.0f, //vertex
    //    0.0f,  0.0f,         //uv
    //    1.0f, -1.0f, -10.0f, //vertex
    //    1.0f,  0.0f,         //uv
    //    0.0f,  1.0f, -10.0f, //vertex
    //    0.5f,  1.0f          //uv
    //};

    int byteSize = (triBuffSize + (textures->size() * 2)) * sizeof(float);

    
    _vertexBuffer = new ResourceBuffer(flattenAttribs, byteSize, _cmdLists[_cmdListIndex], _device);

    _vbv.BufferLocation = _vertexBuffer->getGPUAddress();
    _vbv.StrideInBytes = sizeof(Vertex);
    _vbv.SizeInBytes = byteSize;

   
    //uint16_t indices[] = { 0, 1, 2 };

    int indexBytes = (triBuffSize / 3) * sizeof(uint16_t);
    _indexBuffer = new ResourceBuffer(flattenIndexes, indexBytes, _cmdLists[_cmdListIndex], _device);

    _ibv.BufferLocation = _indexBuffer->getGPUAddress();
    _ibv.Format = DXGI_FORMAT_R16_UINT;
    _ibv.SizeInBytes = indexBytes;

}

