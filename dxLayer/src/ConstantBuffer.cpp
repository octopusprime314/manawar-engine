#include "ConstantBuffer.h"

struct ObjectConstants {
    Matrix mvp;
};

UINT getConstantBufferByteSize(UINT byteSize) {

    return (byteSize + 255) & ~255;
}

ConstantBuffer::ConstantBuffer(ComPtr<ID3D12Device> device, std::vector<D3D12_SHADER_VARIABLE_DESC> constants) {

    UINT bytes = 0;
    for (auto constant : constants) {
        bytes += constant.Size;
    }

    UINT elementsByteSize = getConstantBufferByteSize(bytes);

    device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                                    D3D12_HEAP_FLAG_NONE,
                                    &CD3DX12_RESOURCE_DESC::Buffer(elementsByteSize),
                                    D3D12_RESOURCE_STATE_GENERIC_READ,
                                    nullptr,
                                    IID_PPV_ARGS(&_uploadBuffer));

    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
    cbvHeapDesc.NumDescriptors = 1;
    cbvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask       = 0;

    device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&_cbvHeap));

    D3D12_GPU_VIRTUAL_ADDRESS cbAddress = _uploadBuffer->GetGPUVirtualAddress();

    _cbvDesc.BufferLocation = cbAddress;
    _cbvDesc.SizeInBytes    = getConstantBufferByteSize(bytes);

    device->CreateConstantBufferView(&_cbvDesc, _cbvHeap->GetCPUDescriptorHandleForHeapStart());
}

ConstantBuffer::~ConstantBuffer() {

}

void ConstantBuffer::update(ComPtr<ID3D12GraphicsCommandList> cmdList, 
                            void* data,
                            std::string id,
                            UINT resourceBinding,
                            UINT sizeInBytes,
                            UINT offsetInBytes) {

    
    if (id.compare("objectData") == 0) {
        cmdList->SetGraphicsRoot32BitConstants(0,
                                               sizeInBytes   / 4,
                                               data,
                                               offsetInBytes / 4);
    }
    else {
        BYTE* mappedData = nullptr;
        _uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));

        memcpy(&mappedData[offsetInBytes],
               data,
               sizeInBytes);

        _uploadBuffer->Unmap(0, nullptr);
        mappedData       = nullptr;

        ID3D12DescriptorHeap* descriptorHeaps[] = { _cbvHeap.Get() };
        cmdList->SetDescriptorHeaps(1, descriptorHeaps);

        cmdList->SetGraphicsRootConstantBufferView(resourceBinding,
                                                   _uploadBuffer->GetGPUVirtualAddress());
    }
}
