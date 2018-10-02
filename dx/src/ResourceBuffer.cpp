#include "ResourceBuffer.h"
#include <iostream>
#include <string>
#include "Logger.h"

ResourceBuffer::ResourceBuffer(const void* initData,
    UINT byteSize,
    ComPtr<ID3D12GraphicsCommandList>& cmdList,
    ComPtr<ID3D12Device>& device) {

    device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(byteSize),
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(_defaultBuffer.GetAddressOf()));

    device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(byteSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(_uploadBuffer.GetAddressOf()));

    D3D12_SUBRESOURCE_DATA subResourceData = {};
    subResourceData.pData = initData;
    subResourceData.RowPitch = byteSize;
    subResourceData.SlicePitch = subResourceData.RowPitch;

    cmdList->ResourceBarrier(1,
        &CD3DX12_RESOURCE_BARRIER::Transition(_defaultBuffer.Get(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_COPY_DEST));

    UpdateSubresources<1>(cmdList.Get(), _defaultBuffer.Get(), _uploadBuffer.Get(),
        0, 0, 1, &subResourceData);

    cmdList->ResourceBarrier(1,
        &CD3DX12_RESOURCE_BARRIER::Transition(_defaultBuffer.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_GENERIC_READ));
}

ResourceBuffer::ResourceBuffer(const void* initData,
    UINT byteSize, UINT width, UINT height,
    UINT rowPitch,
    ComPtr<ID3D12GraphicsCommandList>& cmdList,
    ComPtr<ID3D12Device>& device) {
    
    D3D12_SUBRESOURCE_FOOTPRINT pitchedDesc;
    pitchedDesc.Width = width;
    pitchedDesc.Height = height;
    pitchedDesc.Depth = 1;
    auto alignedWidthInBytes = width * sizeof(DWORD);
    auto alignment256 = ((alignedWidthInBytes + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1));
    pitchedDesc.RowPitch = alignment256;
    pitchedDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

    unsigned char* stream = nullptr;
    if (width * height * 4 != pitchedDesc.RowPitch * height ||
        width * height * 4 > byteSize) {

        int channelCount = 3;
        if (byteSize == width * height * 4) {
            channelCount = 4;
        }

        auto byteOffsetPitch = rowPitch - width * channelCount;

        const unsigned char* data = reinterpret_cast<const unsigned char*>(initData);
        stream = new unsigned char[pitchedDesc.RowPitch * height];
        for (UINT i = 0; i < height; i++) {


            //auto message = std::to_string(i * pitchedDesc.RowPitch) + "\n";
            //LOG_INFO(message);

            for (UINT j = 0; j < width; j++) {

                for (int k = 0; k < channelCount; k++) {

                    stream[i*pitchedDesc.RowPitch + (j * 4) + k] = 
                        data[(i*width*channelCount) + (j*channelCount) + k + (i*byteOffsetPitch)];
                    /*if (k == 0) {
                        stream[i*pitchedDesc.RowPitch + (j * 4) + k] = '\xff';
                    }
                    else  if (k == 1) {
                        stream[i*pitchedDesc.RowPitch + (j * 4) + k] = '\x00';
                    }
                    else  if (k == 2) {
                        stream[i*pitchedDesc.RowPitch + (j * 4) + k] = '\x00';
                    }*/
                }
                if (channelCount == 3) {
                    //fill in transparency opaque value
                    stream[i*pitchedDesc.RowPitch + ((j + 1) * 4) - 1] = '\xff'; 
                }
            }
        }
        initData = stream;
        byteSize = pitchedDesc.RowPitch * height;
    }

    //Upload texture data to uploadbuffer
    device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(byteSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(_uploadBuffer.GetAddressOf()));

    UINT8* mappedData = nullptr;
    _uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
    memcpy(mappedData, initData, byteSize);
    _uploadBuffer->Unmap(0, nullptr);
    mappedData = nullptr;

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedTexture2D;
    placedTexture2D.Footprint = pitchedDesc;
    auto alignment512 = ((reinterpret_cast<UINT64>(mappedData) + D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT - 1) & ~(D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT - 1));
    placedTexture2D.Offset = alignment512 - reinterpret_cast<UINT64>(mappedData);

    device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Tex2D(pitchedDesc.Format, width, height, 1, 1), //only 1 mip level for now
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&_defaultBuffer));
    

    cmdList->ResourceBarrier(1,
        &CD3DX12_RESOURCE_BARRIER::Transition(_defaultBuffer.Get(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_COPY_DEST));

    cmdList->CopyTextureRegion(
        &CD3DX12_TEXTURE_COPY_LOCATION(_defaultBuffer.Get(), 0),
        0, 0, 0,
        &CD3DX12_TEXTURE_COPY_LOCATION(_uploadBuffer.Get(), placedTexture2D),
        nullptr);

    cmdList->ResourceBarrier(1,
        &CD3DX12_RESOURCE_BARRIER::Transition(_defaultBuffer.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_GENERIC_READ));
}

D3D12_GPU_VIRTUAL_ADDRESS ResourceBuffer::getGPUAddress() {
    return _defaultBuffer->GetGPUVirtualAddress();
}

D3D12_RESOURCE_DESC ResourceBuffer::getDescriptor() {
    return _defaultBuffer->GetDesc();
}

ID3D12Resource* ResourceBuffer::getResource() {
    return _defaultBuffer.Get();
}



