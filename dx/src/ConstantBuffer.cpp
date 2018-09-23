#include "ConstantBuffer.h"

struct ObjectConstants {
    Matrix mvp;
};

UINT getConstantBufferByteSize(UINT byteSize) {

    return (byteSize + 255) & ~255;
}

ConstantBuffer::ConstantBuffer(ComPtr<ID3D12Device> device) {

    UINT elementsByteSize = getConstantBufferByteSize(sizeof(ObjectConstants));

    device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(elementsByteSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr, IID_PPV_ARGS(&_uploadBuffer));

    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
    cbvHeapDesc.NumDescriptors = 1;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;

    device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&_cbvHeap));

    D3D12_GPU_VIRTUAL_ADDRESS cbAddress = _uploadBuffer->GetGPUVirtualAddress();
    //_cbAddress += getConstantBufferByteSize(sizeof(Matrix));

    _cbvDesc.BufferLocation = cbAddress;
    _cbvDesc.SizeInBytes = getConstantBufferByteSize(sizeof(Matrix));

    device->CreateConstantBufferView(&_cbvDesc, _cbvHeap->GetCPUDescriptorHandleForHeapStart());
}

ConstantBuffer::~ConstantBuffer() {

}

void ConstantBuffer::update(ComPtr<ID3D12GraphicsCommandList> cmdList, 
                            Matrix mvp, 
                            PipelineShader& pso) {

    BYTE* mappedData = nullptr;
    _uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));

    memcpy(mappedData, mvp.getFlatBuffer(), sizeof(Matrix));

    _uploadBuffer->Unmap(0, nullptr);
    mappedData = nullptr;

    ID3D12DescriptorHeap* descriptorHeaps[] = { _cbvHeap.Get() };
    cmdList->SetDescriptorHeaps(1, descriptorHeaps);

    CD3DX12_GPU_DESCRIPTOR_HANDLE cbv(_cbvHeap->GetGPUDescriptorHandleForHeapStart());
    cbv.Offset(0, _cbvDesc.SizeInBytes);

    auto resourceBindings = pso.getResourceBindings();

    cmdList->SetGraphicsRootConstantBufferView(resourceBindings["mvp"], _uploadBuffer->GetGPUVirtualAddress());

}
