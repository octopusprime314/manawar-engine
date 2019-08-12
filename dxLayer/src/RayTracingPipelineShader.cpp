#include "RayTracingPipelineShader.h"
#include "DXLayer.h"
#include "D3D12RaytracingHelpers.hpp"
#include "EngineManager.h"
#include "ShaderTable.h"

const wchar_t* c_hitGroupName = L"MyHitGroup";
const wchar_t* c_raygenShaderName = L"MyRaygenShader";
const wchar_t* c_closestHitShaderName = L"MyClosestHitShader";
const wchar_t* c_missShaderName = L"MyMissShader";

inline std::string BlobToUtf8(_In_ IDxcBlob *pBlob) {
    if (pBlob == nullptr) {
        return std::string();
    }
    return std::string((char *)pBlob->GetBufferPointer(), pBlob->GetBufferSize());
}

inline void AllocateUploadBuffer(ID3D12Device*      pDevice,
                                 void*              pData,
                                 UINT64             datasize,
                                 ID3D12Resource**   ppResource,
                                 const wchar_t*     resourceName = nullptr)
{
    auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto bufferDesc           = CD3DX12_RESOURCE_DESC::Buffer(datasize);

    pDevice->CreateCommittedResource(&uploadHeapProperties,
                                     D3D12_HEAP_FLAG_NONE,
                                     &bufferDesc,
                                     D3D12_RESOURCE_STATE_GENERIC_READ,
                                     nullptr,
                                     IID_PPV_ARGS(ppResource));
    if (resourceName) {
        (*ppResource)->SetName(resourceName);
    }
    void *pMappedData;
    (*ppResource)->Map(0, nullptr, &pMappedData);
    memcpy(pMappedData, pData, datasize);
    (*ppResource)->Unmap(0, nullptr);
}

void RayTracingPipelineShader::_populateDefaultHeap(GpuToCpuBuffers& resources, UINT64 byteSize) {
    // The output buffer (created below) is on a default heap, so only the GPU can access it.

    D3D12_HEAP_PROPERTIES defaultHeapProperties{ CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT) };

    D3D12_RESOURCE_DESC outputBufferDesc{        CD3DX12_RESOURCE_DESC::Buffer(byteSize,
                                                 D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) };

    _dxrDevice->CreateCommittedResource(         &defaultHeapProperties,
                                                 D3D12_HEAP_FLAG_NONE,
                                                 &outputBufferDesc,
                                                 D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
                                                 nullptr,
                                                 IID_PPV_ARGS(&resources.outputBuffer));
}

void RayTracingPipelineShader::_gpuToCpuTransfer(GpuToCpuBuffers& resources,
                                                 UINT64 byteSize) {
    // The readback buffer (created below) is on a readback heap, so that the CPU can access it.

    D3D12_HEAP_PROPERTIES readbackHeapProperties{ CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK) };

    D3D12_RESOURCE_DESC readbackBufferDesc{       CD3DX12_RESOURCE_DESC::Buffer(byteSize) };

    _dxrDevice->CreateCommittedResource(          &readbackHeapProperties,
                                                  D3D12_HEAP_FLAG_NONE,
                                                  &readbackBufferDesc,
                                                  D3D12_RESOURCE_STATE_COPY_DEST,
                                                  nullptr,
                                                  IID_PPV_ARGS(&resources.readbackBuffer));

    D3D12_RESOURCE_BARRIER outputBufferResourceBarrier
    {
        CD3DX12_RESOURCE_BARRIER::Transition(resources.outputBuffer.Get(),
                                             D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
                                             D3D12_RESOURCE_STATE_COPY_SOURCE)
    };
    _dxrCommandList->ResourceBarrier(1,
                                     &outputBufferResourceBarrier);

    _dxrCommandList->CopyResource(resources.readbackBuffer.Get(),
                                  resources.outputBuffer.Get());
}

void RayTracingPipelineShader::_readBackOnCpu(GpuToCpuBuffers& buffers,
                                              UINT64 byteSize) {
    buffers.cpuSideData = new unsigned char[byteSize];
    UINT sizeOfAS       = 0;

    D3D12_RANGE readbackBufferRange{ 0, byteSize };
    buffers.readbackBuffer->Map(0, &readbackBufferRange, (void**)&buffers.cpuSideData);
    memcpy(&sizeOfAS, &(static_cast<unsigned char*>(buffers.cpuSideData))[0], 4);
    OutputDebugString(("Readback on cpu buffer value: " + std::to_string(sizeOfAS)).c_str());
}
RayTracingPipelineShader::RayTracingPipelineShader(std::string shader,
                                                   ComPtr<ID3D12Device> device,
                                                   DXGI_FORMAT format,
                                                   std::vector<Entity*>& entityList) {

    auto commandList      = DXLayer::instance()->getCmdList();
    auto dxLayer          = DXLayer::instance();
    _descriptorsAllocated = 0;
    _raytracingOutput     = new RenderTexture(IOEventDistributor::screenPixelWidth,
                                              IOEventDistributor::screenPixelHeight,
                                              TextureFormat::RGBA_FLOAT);

    device->QueryInterface(     IID_PPV_ARGS(&_dxrDevice));
    commandList->QueryInterface(IID_PPV_ARGS(&_dxrCommandList));

    // Global Root Signature
    // This is a root signature that is shared across all raytracing shaders invoked during a DispatchRays() call.
    {
        ComPtr<ID3DBlob> blob;
        ComPtr<ID3DBlob> error;
        CD3DX12_DESCRIPTOR_RANGE ranges[3]; // Perfomance TIP: Order from most frequent to least frequent.
        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);  // 1 output texture
        ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);  // 1 static index buffers.
        ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);  // 1 static vertex buffers.

        CD3DX12_ROOT_PARAMETER rootParameters[GlobalRootSignatureParams::Count];
        rootParameters[GlobalRootSignatureParams::OutputViewSlot].InitAsDescriptorTable(1, &ranges[0]);
        rootParameters[GlobalRootSignatureParams::AccelerationStructureSlot].InitAsShaderResourceView(0);
        rootParameters[GlobalRootSignatureParams::SceneConstantSlot].InitAsConstantBufferView(0);
        rootParameters[GlobalRootSignatureParams::IndexBuffersSlot].InitAsDescriptorTable(1, &ranges[1]);
        rootParameters[GlobalRootSignatureParams::VertexBuffersSlot].InitAsDescriptorTable(1, &ranges[2]);
        CD3DX12_ROOT_SIGNATURE_DESC globalRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);

        D3D12SerializeRootSignature(&globalRootSignatureDesc,
                                    D3D_ROOT_SIGNATURE_VERSION_1,
                                    &blob,
                                    &error);
        _dxrDevice->CreateRootSignature(1,
                                        blob->GetBufferPointer(),
                                        blob->GetBufferSize(),
                                        IID_PPV_ARGS(&(_raytracingGlobalRootSignature)));
    }

    // Local Root Signature
    // This is a root signature that enables a shader to have unique arguments that come from shader tables.
    {
        ComPtr<ID3DBlob> blob;
        ComPtr<ID3DBlob> error;
        CD3DX12_ROOT_PARAMETER rootParameters[LocalRootSignatureParams::Count];
        rootParameters[LocalRootSignatureParams::CubeConstantSlot].InitAsConstants(/*SizeOfInUint32(_cubeCB)*/16, 1);
        CD3DX12_ROOT_SIGNATURE_DESC localRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);
        localRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;

        D3D12SerializeRootSignature(&localRootSignatureDesc,
                                    D3D_ROOT_SIGNATURE_VERSION_1,
                                    &blob,
                                    &error);

        _dxrDevice->CreateRootSignature(1,
                                        blob->GetBufferPointer(),
                                        blob->GetBufferSize(),
                                        IID_PPV_ARGS(&(_raytracingLocalRootSignature)));
    }

    // Create 7 subobjects that combine into a RTPSO:
    // Subobjects need to be associated with DXIL exports (i.e. shaders) either by way of default or explicit associations.
    // Default association applies to every exported shader entrypoint that doesn't have any of the same type of subobject associated with it.
    // This simple sample utilizes default shader association except for local root signature subobject
    // which has an explicit association specified purely for demonstration purposes.
    // 1 - DXIL library
    // 1 - Triangle hit group
    // 1 - Shader config
    // 2 - Local root signature and association
    // 1 - Global root signature
    // 1 - Pipeline config
    CD3D12_STATE_OBJECT_DESC raytracingPipeline{ D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE };

    HRESULT                     initDLL         = S_FALSE;
    std::string                 shadersLocation = SHADERS_LOCATION;

    ComPtr<IDxcOperationResult> dxcResult;
    ComPtr<IDxcBlob>            dxcBlob;
    CComPtr<IDxcBlobEncoding>   pSource;
    CComPtr<IDxcLibrary>        pLibrary;
    CComPtr<IDxcCompiler2>      dxcCompiler;
    TCHAR                       fullPath[MAX_PATH];

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        shadersLocation += "glsl/";
    }
    else {
        shadersLocation += "hlsl/";
    }
    std::wstring fullPathWstring = stringToLPCWSTR((shadersLocation + "rayTracingUberShader.hlsl").c_str());
    GetFullPathName((shadersLocation + "rayTracingUberShader.hlsl").c_str(), MAX_PATH, fullPath, NULL);
    fullPathWstring              = stringToLPCWSTR(fullPath);

    if (!_dllSupport.IsEnabled()) {
        initDLL = _dllSupport.Initialize();
    }

    _dllSupport.CreateInstance(CLSID_DxcLibrary, &pLibrary);
    pLibrary  ->CreateBlobFromFile(fullPathWstring.c_str(), nullptr, &pSource);
    _dllSupport.CreateInstance(CLSID_DxcCompiler, &dxcCompiler);

    std::string libProfile = "lib_6_3";
    if (EngineManager::getGraphicsLayer() == GraphicsLayer::DXR_EXPERIMENTAL) {
        libProfile = "lib_6_5";
       
    }
    CA2W shWide(libProfile.c_str(), CP_UTF8);
    dxcCompiler->Compile(pSource,
                         L"rayTracingUberShader.hlsl",
                         L"",
                         shWide,
                         nullptr, 0, nullptr, 0, nullptr, &dxcResult);

    ComPtr<IDxcBlob> pResultBlob;
    dxcResult->GetResult(&pResultBlob);

    HRESULT result;
    dxcResult->GetStatus(&result);
    if (FAILED(result)) {
        CComPtr<IDxcBlobEncoding> pErr;
        dxcResult->GetErrorBuffer(&pErr);
        OutputDebugStringA((char*)pErr->GetBufferPointer());
    }

    CComPtr<IDxcBlob>         pProgram;
    CComPtr<IDxcBlobEncoding> pDisassembleBlob;

    dxcResult  ->GetResult(&pProgram);
    dxcCompiler->Disassemble(pProgram, &pDisassembleBlob);

    std::string disassembleString((const char *)pDisassembleBlob->GetBufferPointer());

    //// This contains the shaders and their entrypoints for the state object.
    //// Since shaders are not considered a subobject, they need to be passed in via DXIL library subobjects.
    auto lib = raytracingPipeline.CreateSubobject<CD3D12_DXIL_LIBRARY_SUBOBJECT>();
    D3D12_SHADER_BYTECODE libdxil = CD3DX12_SHADER_BYTECODE((unsigned char *)pProgram->GetBufferPointer(),
                                                                             pProgram->GetBufferSize());
    lib->SetDXILLibrary(&libdxil);
    // Define which shader exports to surface from the library.
    // If no shader exports are defined for a DXIL library subobject, all shaders will be surfaced.
    // In this sample, this could be omitted for convenience since the sample uses all shaders in the library. 
    {
        lib->DefineExport(c_raygenShaderName);
        lib->DefineExport(c_closestHitShaderName);
        lib->DefineExport(c_missShaderName);
    }

    // Triangle hit group
    // A hit group specifies closest hit, any hit and intersection shaders to be executed when a ray intersects the geometry's triangle/AABB.
    // In this sample, we only use triangle geometry with a closest hit shader, so others are not set.
    auto hitGroup = raytracingPipeline.CreateSubobject<CD3D12_HIT_GROUP_SUBOBJECT>();
    hitGroup->SetClosestHitShaderImport(c_closestHitShaderName);
    hitGroup->SetHitGroupExport(c_hitGroupName);
    hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

    // Shader config
    // Defines the maximum sizes in bytes for the ray payload and attribute structure.
    auto shaderConfig = raytracingPipeline.CreateSubobject<CD3D12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
    UINT payloadSize = 4 * sizeof(float);   // float4 color
    UINT attributeSize = 2 * sizeof(float); // float2 barycentrics
    shaderConfig->Config(payloadSize, attributeSize);

    // Ray gen and miss shaders in this sample are not using a local root signature and thus one is not associated with them.

   // Local root signature to be used in a hit group.
    auto localRootSignature = raytracingPipeline.CreateSubobject<CD3D12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
    localRootSignature->SetRootSignature(_raytracingLocalRootSignature.Get());
    // Define explicit shader association for the local root signature. 
    {
        auto rootSignatureAssociation = raytracingPipeline.CreateSubobject<CD3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
        rootSignatureAssociation->SetSubobjectToAssociate(*localRootSignature);
        rootSignatureAssociation->AddExport(c_hitGroupName);
    }

    // This is a root signature that enables a shader to have unique arguments that come from shader tables.

    // Global root signature
    // This is a root signature that is shared across all raytracing shaders invoked during a DispatchRays() call.
    auto globalRootSignature = raytracingPipeline.CreateSubobject<CD3D12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
    globalRootSignature->SetRootSignature(_raytracingGlobalRootSignature.Get());

    // Pipeline config
    // Defines the maximum TraceRay() recursion depth.
    auto pipelineConfig    = raytracingPipeline.CreateSubobject<CD3D12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
    // PERFOMANCE TIP: Set max recursion depth as low as needed 
    // as drivers may apply optimization strategies for low recursion depths. 
    UINT maxRecursionDepth = 1; // ~ primary rays only. 
    pipelineConfig->Config(maxRecursionDepth);

    _dxrDevice->CreateStateObject(raytracingPipeline, IID_PPV_ARGS(&_dxrStateObject));

    std::map<std::string, std::vector<Entity*>> bottomLevelInstances;
    std::map<std::string, Entity*>              bottomLevelModels;
    int                                         testCount     = 0;
    UINT                                        instanceCount = 0;
    for (auto entity : entityList) {
        std::string name = entity->getModel()->getName();
        bottomLevelInstances[name].push_back(entity);
        bottomLevelModels[name] = entity;
        testCount++;
    }
    for (auto instances : bottomLevelInstances) {
        instanceCount += static_cast<UINT>(instances.second.size());
    }

    //D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
    // Allocate a heap for N + 3 descriptors:
    // 2 - vertex and index buffer SRVs
    // 1 - raytracing output texture SRV
    // N - bottom and top level acceleration structure fallback wrapped pointer UAVs
    _descriptorHeapDesc.NumDescriptors = static_cast<UINT>(3 + bottomLevelModels.size() + instanceCount);
    _descriptorHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    _descriptorHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    _descriptorHeapDesc.NodeMask       = 0;
    _dxrDevice->CreateDescriptorHeap(&_descriptorHeapDesc, IID_PPV_ARGS(&_descriptorHeap));

    _descriptorSize = _dxrDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    _geometryDesc = new D3D12_RAYTRACING_GEOMETRY_DESC[bottomLevelModels.size()];
    UINT modelIndex = 0;
    struct Vertex {
        float pos[3];
        float normal[3];
        float uv[2];
    };

    for (auto entity : bottomLevelModels) {

        auto indexGPUAddress = (*entity.second->getFrustumVAO())[0]->getIndexResource()->getResource()->GetGPUVirtualAddress();
        auto vertexGPUAddress = (*entity.second->getFrustumVAO())[0]->getVertexResource()->getResource()->GetGPUVirtualAddress();

        auto indexDesc = (*entity.second->getFrustumVAO())[0]->getIndexResource()->getResource()->GetDesc();
        auto vertexDesc = (*entity.second->getFrustumVAO())[0]->getVertexResource()->getResource()->GetDesc();

        _geometryDesc[modelIndex].Type                                 = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
        _geometryDesc[modelIndex].Triangles.IndexBuffer                = indexGPUAddress;
        _geometryDesc[modelIndex].Triangles.IndexCount                 = static_cast<UINT>(indexDesc.Width) / sizeof(uint32_t);
        _geometryDesc[modelIndex].Triangles.IndexFormat                = DXGI_FORMAT_R32_UINT;
        _geometryDesc[modelIndex].Triangles.Transform3x4               = 0;
        _geometryDesc[modelIndex].Triangles.VertexFormat               = DXGI_FORMAT_R32G32B32_FLOAT;
        _geometryDesc[modelIndex].Triangles.VertexCount                = static_cast<UINT>(vertexDesc.Width) / sizeof(Vertex);
        _geometryDesc[modelIndex].Triangles.VertexBuffer.StartAddress  = vertexGPUAddress;
        _geometryDesc[modelIndex].Triangles.VertexBuffer.StrideInBytes = sizeof(Vertex);

        _indexBuffer.push_back(D3DBuffer());
        _vertexBuffer.push_back(D3DBuffer());
        _indexBuffer[modelIndex].resource  = (*entity.second->getFrustumVAO())[0]->getIndexResource()->getResource();
        _vertexBuffer[modelIndex].resource = (*entity.second->getFrustumVAO())[0]->getVertexResource()->getResource();
        
        // Vertex buffer is passed to the shader along with index buffer as a descriptor table.
        // Vertex buffer descriptor must follow index buffer descriptor in the descriptor heap.
        UINT descriptorIndexIB = _createBufferSRV(&_indexBuffer[modelIndex],
                                                  _geometryDesc[modelIndex].Triangles.IndexCount,
                                                  0);
        UINT descriptorIndexVB = _createBufferSRV(&_vertexBuffer[modelIndex],
                                                  _geometryDesc[modelIndex].Triangles.VertexCount,
                                                  static_cast<UINT>(_geometryDesc[modelIndex].Triangles.VertexBuffer.StrideInBytes));

        // Mark the geometry as opaque. 
        // PERFORMANCE TIP: mark geometry as opaque whenever applicable as it can enable important ray processing optimizations.
        // Note: When rays encounter opaque geometry an any hit shader will not be executed whether it is present or not.
        _geometryDesc[modelIndex].Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
        modelIndex++;
    }

    // Get required sizes for an acceleration structure.
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottomLevelBuildDesc = {};
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS &bottomLevelInputs = bottomLevelBuildDesc.Inputs;
    bottomLevelInputs.DescsLayout                                           = D3D12_ELEMENTS_LAYOUT_ARRAY;
    bottomLevelInputs.Flags                                                 = buildFlags;
    bottomLevelInputs.NumDescs                                              = static_cast<UINT>(bottomLevelModels.size());
    bottomLevelInputs.Type                                                  = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
    bottomLevelInputs.pGeometryDescs                                        = _geometryDesc;

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC topLevelBuildDesc    = bottomLevelBuildDesc;
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS &topLevelInputs    = topLevelBuildDesc.Inputs;
    topLevelInputs.DescsLayout                                              = D3D12_ELEMENTS_LAYOUT_ARRAY;
    topLevelInputs.Flags                                                    = buildFlags;
    topLevelInputs.NumDescs                                                 = static_cast<UINT>(instanceCount);
    topLevelInputs.pGeometryDescs                                           = nullptr;
    topLevelInputs.Type                                                     = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO topLevelPrebuildInfo = {};

    _dxrDevice->GetRaytracingAccelerationStructurePrebuildInfo(&topLevelInputs,
                                                               &topLevelPrebuildInfo);

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO bottomLevelPrebuildInfo = {};
    _dxrDevice->GetRaytracingAccelerationStructurePrebuildInfo(&bottomLevelInputs, &bottomLevelPrebuildInfo);

    auto sizeBuildInfo        = max(topLevelPrebuildInfo.ScratchDataSizeInBytes, bottomLevelPrebuildInfo.ScratchDataSizeInBytes);
    auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    auto bufferDesc           = CD3DX12_RESOURCE_DESC::Buffer(sizeBuildInfo, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    
    _dxrDevice->CreateCommittedResource(&uploadHeapProperties,
                                    D3D12_HEAP_FLAG_NONE,
                                    &bufferDesc,
                                    D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
                                    nullptr,
                                    IID_PPV_ARGS(&_scratchResource));

    modelIndex = 0;
    for (auto model : bottomLevelInstances) {
        // Allocate resources for acceleration structures.
        // Acceleration structures can only be placed in resources that are created in the default heap (or custom heap equivalent). 
        // Default heap is OK since the application doesn’t need CPU read/write access to them. 
        // The resources that will contain acceleration structures must be created in the state D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, 
        // and must have resource flag D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS. The ALLOW_UNORDERED_ACCESS requirement simply acknowledges both: 
        //  - the system will be doing this type of access in its implementation of acceleration structure builds behind the scenes.
        //  - from the app point of view, synchronization of writes/reads to acceleration structures is accomplished using UAV barriers.
        
        _bottomLevelAccelerationStructure.push_back(ComPtr<ID3D12Resource>());
        D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
        auto sizeBuildInfo                         = bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes;
        auto uploadHeapProperties                  = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        auto bufferDesc                            = CD3DX12_RESOURCE_DESC::Buffer(sizeBuildInfo,
                                                                                   D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
        _dxrDevice->CreateCommittedResource(&uploadHeapProperties,
                                            D3D12_HEAP_FLAG_NONE,
                                            &bufferDesc,
                                            initialResourceState,
                                            nullptr,
                                            IID_PPV_ARGS(&(_bottomLevelAccelerationStructure[modelIndex])));
        modelIndex++;
    }

    {
        D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
        auto sizeBuildInfo                         = topLevelPrebuildInfo.ResultDataMaxSizeInBytes;
        auto uploadHeapProperties                  = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        auto bufferDesc                            = CD3DX12_RESOURCE_DESC::Buffer(sizeBuildInfo,
                                                                                   D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
        _dxrDevice->CreateCommittedResource(&uploadHeapProperties,
                                            D3D12_HEAP_FLAG_NONE,
                                            &bufferDesc,
                                            initialResourceState,
                                            nullptr,
                                            IID_PPV_ARGS(&_topLevelAccelerationStructure));
    }

    // Create an instance desc for the bottom-level acceleration structure.
    _instanceDesc         = new D3D12_RAYTRACING_INSTANCE_DESC[instanceCount];
    UINT bottomLevelIndex = 0;
    UINT i                = 0;
    for (auto instanceTransform : bottomLevelInstances) {
        
        for (auto instance : instanceTransform.second) {
            memcpy(&_instanceDesc[i].Transform, instance->getWorldSpaceTransform().getFlatBuffer(), sizeof(float) * 12);
            _instanceDesc[i].InstanceMask                        = 1;
            _instanceDesc[i].InstanceContributionToHitGroupIndex = 0;
            _instanceDesc[i].AccelerationStructure = 
                (_bottomLevelAccelerationStructure[bottomLevelIndex])->GetGPUVirtualAddress();
            i++;
        }
        bottomLevelIndex++;
    }

    AllocateUploadBuffer(_dxrDevice.Get(),
                         _instanceDesc,
                         sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * instanceCount,
                         &_instanceDescs,
                         L"InstanceDescs");

    // Top Level Acceleration Structure desc
    {
        topLevelBuildDesc.DestAccelerationStructureData    = _topLevelAccelerationStructure->GetGPUVirtualAddress();
        topLevelBuildDesc.ScratchAccelerationStructureData = _scratchResource->GetGPUVirtualAddress();
        topLevelBuildDesc.Inputs.InstanceDescs             = _instanceDescs->GetGPUVirtualAddress();
    }

    GpuToCpuBuffers bottomLevelSerializedAS;
    GpuToCpuBuffers topLevelSerializedAS;
    i = 0;
    for (auto instanceTransform : bottomLevelInstances) {

        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS &bottomLevelInputs = bottomLevelBuildDesc.Inputs;
        bottomLevelInputs.DescsLayout                                           = D3D12_ELEMENTS_LAYOUT_ARRAY;
        bottomLevelInputs.Flags                                                 = buildFlags;
        bottomLevelInputs.NumDescs                                              = 1;
        bottomLevelInputs.Type                                                  = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
        bottomLevelInputs.pGeometryDescs                                        = &_geometryDesc[i];

        bottomLevelBuildDesc.ScratchAccelerationStructureData = _scratchResource->GetGPUVirtualAddress();
        bottomLevelBuildDesc.DestAccelerationStructureData    = 
            (_bottomLevelAccelerationStructure[i])->GetGPUVirtualAddress();
            
        _dxrCommandList->BuildRaytracingAccelerationStructure(&bottomLevelBuildDesc,
                                                              0,
                                                              nullptr);

        _dxrCommandList->ResourceBarrier(1,
                                         &CD3DX12_RESOURCE_BARRIER::UAV((_bottomLevelAccelerationStructure[i]).Get()));
        i++;
    }

    _dxrCommandList->BuildRaytracingAccelerationStructure(&topLevelBuildDesc, 0, nullptr);
    _dxrCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::UAV(_topLevelAccelerationStructure.Get()));

    // Allocate one constant buffer per frame, since it gets updated every frame.
    size_t cbSize                                = 2 * sizeof(AlignedSceneConstantBuffer);
    const D3D12_RESOURCE_DESC constantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(cbSize);
    auto uploadHeapProps                         = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    _dxrDevice->CreateCommittedResource(&uploadHeapProps,
                                        D3D12_HEAP_FLAG_NONE,
                                        &constantBufferDesc,
                                        D3D12_RESOURCE_STATE_GENERIC_READ,
                                        nullptr,
                                        IID_PPV_ARGS(&_perFrameConstants));

    // Map the constant buffer and cache its heap pointers.
    // We don't unmap this until the app closes. Keeping buffer mapped for the lifetime of the resource is okay.
    CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
    _perFrameConstants->Map(0, nullptr, reinterpret_cast<void**>(&_mappedConstantData));

    void* rayGenShaderIdentifier;
    void* missShaderIdentifier;
    void* hitGroupShaderIdentifier;

    auto GetShaderIdentifiers = [&](auto* stateObjectProperties)
    {
        rayGenShaderIdentifier   = stateObjectProperties->GetShaderIdentifier(c_raygenShaderName);
        missShaderIdentifier     = stateObjectProperties->GetShaderIdentifier(c_missShaderName);
        hitGroupShaderIdentifier = stateObjectProperties->GetShaderIdentifier(c_hitGroupName);
    };

    // Get shader identifiers.
    UINT shaderIdentifierSize;
    ComPtr<ID3D12StateObjectPropertiesPrototype> stateObjectProperties;
    _dxrStateObject.As(&stateObjectProperties);
    GetShaderIdentifiers(stateObjectProperties.Get());
    shaderIdentifierSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;

    // Ray gen shader table
    {
        UINT numShaderRecords = 1;
        UINT shaderRecordSize = shaderIdentifierSize;
        ShaderTable rayGenShaderTable(_dxrDevice.Get(), numShaderRecords, shaderRecordSize, L"RayGenShaderTable");
        rayGenShaderTable.pushBack(ShaderRecord(rayGenShaderIdentifier, shaderIdentifierSize));
        _rayGenShaderTable    = rayGenShaderTable.GetResource();
    }

    // Miss shader table
    {
        UINT numShaderRecords = 1;
        UINT shaderRecordSize = shaderIdentifierSize;
        ShaderTable missShaderTable(_dxrDevice.Get(), numShaderRecords, shaderRecordSize, L"MissShaderTable");
        missShaderTable.pushBack(ShaderRecord(missShaderIdentifier, shaderIdentifierSize));
        _missShaderTable      = missShaderTable.GetResource();
    }

    // Hit group shader table
    {
        
        UINT numShaderRecords = 1;
        UINT shaderRecordSize = shaderIdentifierSize;
        ShaderTable hitGroupShaderTable(_dxrDevice.Get(), numShaderRecords, shaderRecordSize, L"HitGroupShaderTable");
        hitGroupShaderTable.pushBack(ShaderRecord(hitGroupShaderIdentifier, shaderIdentifierSize));
        _hitGroupShaderTable  = hitGroupShaderTable.GetResource();
    }

    //Create view of UAV for shader access
    _hUAVDescriptor = new CD3DX12_CPU_DESCRIPTOR_HANDLE(_descriptorHeap->GetCPUDescriptorHandleForHeapStart(),
                                                        0,
                                                        _descriptorSize);
    _uavDesc                    = {};
    _uavDesc.Format             = DXGI_FORMAT_R32G32B32A32_FLOAT;
    _uavDesc.ViewDimension      = D3D12_UAV_DIMENSION_TEXTURE2D;
    _uavDesc.Texture2D.MipSlice = 0;
    _dxrDevice->CreateUnorderedAccessView(_raytracingOutput->getResource()->getResource().Get(),
                                      nullptr,
                                      &_uavDesc,
                                      *_hUAVDescriptor);
}

void RayTracingPipelineShader::_queryShaderResources(ComPtr<ID3DBlob> shaderBlob) {

    ID3D12ShaderReflection* reflectionInterface;
    D3DReflect(shaderBlob->GetBufferPointer(),
               shaderBlob->GetBufferSize(),
               IID_ID3D12ShaderReflection,
               (void**)&reflectionInterface);

    HRESULT result = S_OK;
    PipelineShader::_queryShaderResources(shaderBlob);
}

void RayTracingPipelineShader::doRayTracing(Entity* entity, Light* light) {

    auto commandList       = DXLayer::instance()->getCmdList();

    auto inverseProj       = (light->getLightMVP().getProjectionMatrix() *
                              light->getLightMVP().getViewMatrix() *
                              light->getLightMVP().getModelMatrix()).inverse();

    auto inverseCameraView = (entity->getMVP()->getViewMatrix() *
                              entity->getMVP()->getModelMatrix()).inverse();

    auto inverseLightView  = (light->getLightMVP().getViewMatrix() *
                              light->getLightMVP().getModelMatrix()).inverse();

    auto lightDir          = light->getLightDirection();

    memcpy(&(_sceneCB[0]), inverseProj.getFlatBuffer(), sizeof(float) * 16);

    _sceneCB[0].cameraPosition[0] = inverseCameraView.getFlatBuffer()[3];
    _sceneCB[0].cameraPosition[1] = inverseCameraView.getFlatBuffer()[7];
    _sceneCB[0].cameraPosition[2] = inverseCameraView.getFlatBuffer()[11];
    _sceneCB[0].cameraPosition[3] = 1.0;
    _sceneCB[0].lightPosition[0]  = inverseLightView.getFlatBuffer()[3];
    _sceneCB[0].lightPosition[1]  = inverseLightView.getFlatBuffer()[7];
    _sceneCB[0].lightPosition[2]  = inverseLightView.getFlatBuffer()[11];
    _sceneCB[0].lightPosition[3]  = 1.0f;
    _sceneCB[0].lightDirection[0] = lightDir.getx();
    _sceneCB[0].lightDirection[1] = lightDir.gety();
    _sceneCB[0].lightDirection[2] = lightDir.getz();
    _sceneCB[0].lightDirection[3] = 1.0f;

    auto projection      = light->getLightMVP().getProjectionMatrix().getFlatBuffer();
    memcpy(&(_sceneCB[0].projection), projection, sizeof(float) * 16);

    auto lightViewMatrix = light->getLightMVP().getViewMatrix().getFlatBuffer();
    memcpy(&(_sceneCB[0].lightView), lightViewMatrix, sizeof(float) * 16);

    commandList->QueryInterface(IID_PPV_ARGS(&_dxrCommandList));
    _dxrCommandList->SetComputeRootSignature(_raytracingGlobalRootSignature.Get());
    _dxrCommandList->SetDescriptorHeaps(1, _descriptorHeap.GetAddressOf());
    
    auto gpuDesc = CD3DX12_GPU_DESCRIPTOR_HANDLE(_descriptorHeap->GetGPUDescriptorHandleForHeapStart(), GlobalRootSignatureParams::OutputViewSlot, _descriptorSize);
    _dxrCommandList->SetComputeRootDescriptorTable(GlobalRootSignatureParams::OutputViewSlot, gpuDesc);

    // Copy the updated scene constant buffer to GPU.
    memcpy(&_mappedConstantData[0].constants, &_sceneCB[0], sizeof(_sceneCB[0]));
    auto cbGpuAddress = _perFrameConstants->GetGPUVirtualAddress() + 0 * sizeof(_mappedConstantData[0]);
    _dxrCommandList->SetComputeRootConstantBufferView(GlobalRootSignatureParams::SceneConstantSlot,
                                                      cbGpuAddress);

    _dxrCommandList->SetComputeRootShaderResourceView(GlobalRootSignatureParams::AccelerationStructureSlot,
                                                      _topLevelAccelerationStructure->GetGPUVirtualAddress());

    _dispatchDesc                                        = {};
    _dispatchDesc.HitGroupTable.StartAddress             = _hitGroupShaderTable->GetGPUVirtualAddress();
    _dispatchDesc.HitGroupTable.SizeInBytes              = _hitGroupShaderTable->GetDesc().Width;
    _dispatchDesc.HitGroupTable.StrideInBytes            = _dispatchDesc.HitGroupTable.SizeInBytes;
    _dispatchDesc.MissShaderTable.StartAddress           = _missShaderTable->GetGPUVirtualAddress();
    _dispatchDesc.MissShaderTable.SizeInBytes            = _missShaderTable->GetDesc().Width;
    _dispatchDesc.MissShaderTable.StrideInBytes          = _dispatchDesc.MissShaderTable.SizeInBytes;
    _dispatchDesc.RayGenerationShaderRecord.StartAddress = _rayGenShaderTable->GetGPUVirtualAddress();
    _dispatchDesc.RayGenerationShaderRecord.SizeInBytes  = _rayGenShaderTable->GetDesc().Width;
    _dispatchDesc.Width                                  = IOEventDistributor::screenPixelWidth;
    _dispatchDesc.Height                                 = IOEventDistributor::screenPixelHeight;
    _dispatchDesc.Depth                                  = 1;
    _dxrCommandList->SetPipelineState1(_dxrStateObject.Get());

    _dxrCommandList->SetComputeRootDescriptorTable(GlobalRootSignatureParams::IndexBuffersSlot,
                                                   _indexBuffer[0].gpuDescriptorHandle);

    _dxrCommandList->SetComputeRootDescriptorTable(GlobalRootSignatureParams::VertexBuffersSlot,
                                                   _vertexBuffer[0].gpuDescriptorHandle);
    _dxrCommandList->DispatchRays(&_dispatchDesc);
    _dxrCommandList->ResourceBarrier(1,
                                     &CD3DX12_RESOURCE_BARRIER::UAV(_raytracingOutput->getResource()->getResource().Get()));
}

// Allocate a descriptor and return its index. 
// If the passed descriptorIndexToUse is valid, it will be used instead of allocating a new one.
UINT RayTracingPipelineShader::_allocateDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE* cpuDescriptor, UINT descriptorIndexToUse) {
    auto descriptorHeapCpuBase =  _descriptorHeap->GetCPUDescriptorHandleForHeapStart();

    if (descriptorIndexToUse   >= _descriptorHeap->GetDesc().NumDescriptors) {
        descriptorIndexToUse   =  _descriptorsAllocated++;
    }
    *cpuDescriptor             = CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptorHeapCpuBase, descriptorIndexToUse, _descriptorSize);
    return descriptorIndexToUse;
}
// Create SRV for a buffer.
UINT RayTracingPipelineShader::_createBufferSRV(D3DBuffer* buffer, UINT numElements, UINT elementSize) {
    
    auto device                             = DXLayer::instance()->getDevice();
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.ViewDimension                   = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Buffer.NumElements              = numElements;
    
    if (elementSize == 0) {
        srvDesc.Format                     = DXGI_FORMAT_R32_TYPELESS;
        srvDesc.Buffer.Flags               = D3D12_BUFFER_SRV_FLAG_RAW;
        srvDesc.Buffer.StructureByteStride = 0;
    }
    else {
        srvDesc.Format                     = DXGI_FORMAT_UNKNOWN;
        srvDesc.Buffer.Flags               = D3D12_BUFFER_SRV_FLAG_NONE;
        srvDesc.Buffer.StructureByteStride = elementSize;
    }
    UINT descriptorIndex = _allocateDescriptor(&buffer->cpuDescriptorHandle);
    device->CreateShaderResourceView(buffer->resource.Get(),
                                     &srvDesc,
                                     buffer->cpuDescriptorHandle);
    buffer->gpuDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(_descriptorHeap->GetGPUDescriptorHandleForHeapStart(),
                                                                descriptorIndex,
                                                                _descriptorSize);
    return descriptorIndex;
}

RenderTexture* RayTracingPipelineShader::getRayTracingTarget() {
    return _raytracingOutput;
}