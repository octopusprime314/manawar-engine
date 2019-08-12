#pragma once
#include <wrl.h>
#include "d3d12_1.h"
#include "d3d12.h"
#include "d3dx12.h"
#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <sstream>
using namespace Microsoft::WRL;

inline UINT Align(UINT size, UINT alignment)
{
    return (size + (alignment - 1)) & ~(alignment - 1);
}

// Shader record = {{Shader ID}, {RootArguments}}
class ShaderRecord
{
public:
    ShaderRecord(void* pShaderIdentifier,
                 UINT shaderIdentifierSize) :

        m_ShaderIdentifier(pShaderIdentifier, shaderIdentifierSize) {}

    ShaderRecord(void* pShaderIdentifier,
                 UINT shaderIdentifierSize,
                 void* pLocalRootArguments,
                 UINT localRootArgumentsSize) :

        m_ShaderIdentifier(pShaderIdentifier, shaderIdentifierSize),
        m_LocalRootArguments(pLocalRootArguments, localRootArgumentsSize) {}

    void CopyTo(void* dest) const {
        uint8_t* byteDest = static_cast<uint8_t*>(dest);
        memcpy(byteDest, m_ShaderIdentifier.ptr, m_ShaderIdentifier.size);

        if (m_LocalRootArguments.ptr) {
            memcpy(byteDest + m_ShaderIdentifier.size, m_LocalRootArguments.ptr, m_LocalRootArguments.size);
        }
    }

    struct PointerWithSize {
        void *ptr;
        UINT size;

        PointerWithSize() : ptr(nullptr), size(0) {}
        PointerWithSize(void* _ptr, UINT _size) : ptr(_ptr), size(_size) {};
    };

    PointerWithSize m_ShaderIdentifier;
    PointerWithSize m_LocalRootArguments;
};

// Shader table = {{ ShaderRecord 1}, {ShaderRecord 2}, ...}
class ShaderTable
{
    uint8_t*                  m_mappedShaderRecords;
    UINT                      m_shaderRecordSize;
    ComPtr<ID3D12Resource>    m_resource;

    // Debug support
    std::wstring              m_name;
    std::vector<ShaderRecord> m_shaderRecords;

    ShaderTable() {}
public:


    uint8_t* MapCpuWriteOnly()
    {
        uint8_t*      mappedData;
        // We don't unmap this until the app closes. Keeping buffer mapped for the lifetime of the resource is okay.
        // We do not intend to read from this resource on the CPU.
        CD3DX12_RANGE readRange(0, 0);
        m_resource->Map(0, &readRange, reinterpret_cast<void**>(&mappedData));
        return mappedData;
    }

    void Allocate(ID3D12Device* device,
                  UINT bufferSize,
                  LPCWSTR resourceName = nullptr) {

        auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto bufferDesc           = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
        
        device->CreateCommittedResource(&uploadHeapProperties,
                                        D3D12_HEAP_FLAG_NONE,
                                        &bufferDesc,
                                        D3D12_RESOURCE_STATE_GENERIC_READ,
                                        nullptr,
                                        IID_PPV_ARGS(&m_resource));
        
        m_resource->SetName(resourceName);
    }

    ShaderTable(ID3D12Device* device,
                UINT numShaderRecords,
                UINT shaderRecordSize,
                LPCWSTR resourceName = nullptr)
        : m_name(resourceName) {

        m_shaderRecordSize    = Align(shaderRecordSize, D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT);
        UINT bufferSize       = numShaderRecords * m_shaderRecordSize;
        Allocate(device, bufferSize, resourceName);

        m_shaderRecords.reserve(numShaderRecords);
        m_mappedShaderRecords = MapCpuWriteOnly();

    }

    void pushBack(const ShaderRecord& shaderRecord) {
        m_shaderRecords.push_back(shaderRecord);
        shaderRecord.CopyTo(m_mappedShaderRecords);

        m_mappedShaderRecords += m_shaderRecordSize;
    }

    UINT GetShaderRecordSize() {
        return m_shaderRecordSize;
    }

    // Pretty-print the shader records.
    void DebugPrint(std::unordered_map<void*, std::wstring> shaderIdToStringMap)
    {
        std::wstringstream wstr;
        wstr << L"|--------------------------------------------------------------------\n";
        wstr << L"|Shader table - " << m_name.c_str() << L": "
            << m_shaderRecordSize << L" | "
            << m_shaderRecords.size() * m_shaderRecordSize << L" bytes\n";

        for (UINT i = 0; i < m_shaderRecords.size(); i++)
        {
            wstr << L"| [" << i << L"]: ";
            wstr << shaderIdToStringMap[m_shaderRecords[i].m_ShaderIdentifier.ptr] << L", ";
            wstr << m_shaderRecords[i].m_ShaderIdentifier.size
                 << L" + "
                 << m_shaderRecords[i].m_LocalRootArguments.size
                 << L" bytes \n";
        }
        wstr << L"|--------------------------------------------------------------------\n";
        wstr << L"\n";
        OutputDebugStringW(wstr.str().c_str());
    }

    ComPtr<ID3D12Resource> GetResource() {
        return m_resource;
    }

};