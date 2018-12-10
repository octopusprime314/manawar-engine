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
    if (pBlob == nullptr)
        return std::string();
    return std::string((char *)pBlob->GetBufferPointer(), pBlob->GetBufferSize());
}

RayTracingPipelineShader::RayTracingPipelineShader(std::string shader,
    ComPtr<ID3D12Device> device,
    DXGI_FORMAT format, Entity* entity) {

    auto commandList = DXLayer::instance()->getCmdList();

    device->QueryInterface(IID_PPV_ARGS(&_dxrDevice));
    commandList->QueryInterface(IID_PPV_ARGS(&_dxrCommandList));

    ComPtr<ID3DBlob> blob;
    ComPtr<ID3DBlob> error;

    // Global Root Signature
    // This is a root signature that is shared across all raytracing shaders invoked during a DispatchRays() call.
    {
        CD3DX12_DESCRIPTOR_RANGE ranges[2]; // Perfomance TIP: Order from most frequent to least frequent.
        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);  // 1 output texture
        ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 1);  // 2 static index and vertex buffers.

        CD3DX12_ROOT_PARAMETER rootParameters[GlobalRootSignatureParams::Count];
        rootParameters[GlobalRootSignatureParams::OutputViewSlot].InitAsDescriptorTable(1, &ranges[0]);
        rootParameters[GlobalRootSignatureParams::AccelerationStructureSlot].InitAsShaderResourceView(0);
        rootParameters[GlobalRootSignatureParams::SceneConstantSlot].InitAsConstantBufferView(0);
        rootParameters[GlobalRootSignatureParams::VertexBuffersSlot].InitAsDescriptorTable(1, &ranges[1]);
        CD3DX12_ROOT_SIGNATURE_DESC globalRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);
        D3D12SerializeRootSignature(&globalRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error);
        device->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&(_raytracingGlobalRootSignature)));
    }

    // Local Root Signature
    // This is a root signature that enables a shader to have unique arguments that come from shader tables.
    {
        CD3DX12_ROOT_PARAMETER rootParameters[LocalRootSignatureParams::Count];
        rootParameters[LocalRootSignatureParams::CubeConstantSlot].InitAsConstants(SizeOfInUint32(_cubeCB), 1);
        CD3DX12_ROOT_SIGNATURE_DESC localRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);
        localRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
        D3D12SerializeRootSignature(&localRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error);
        device->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&(_raytracingLocalRootSignature)));
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

    ComPtr<IDxcOperationResult> dxcResult;
    ComPtr<IDxcBlob> dxcBlob;

    std::string shadersLocation = SHADERS_LOCATION;
    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        shadersLocation += "glsl/";
    }
    else {
        shadersLocation += "hlsl/";
    }
    TCHAR full_path[MAX_PATH];
    std::wstring fullPath = stringToLPCWSTR((shadersLocation + "rayTracingUberShader.hlsl").c_str());
    GetFullPathName((shadersLocation + "rayTracingUberShader.hlsl").c_str(), MAX_PATH, full_path, NULL);
    fullPath = stringToLPCWSTR(full_path);
    CComPtr<IDxcBlobEncoding> pSource;
    CComPtr<IDxcLibrary> pLibrary;


    HRESULT initDLL = S_FALSE;
    if (!_dllSupport.IsEnabled()) {
        initDLL = _dllSupport.Initialize();
    }

    _dllSupport.CreateInstance(CLSID_DxcLibrary, &pLibrary);
    pLibrary->CreateBlobFromFile(fullPath.c_str(), nullptr, &pSource);

    CComPtr<IDxcCompiler2> dxcCompiler;
    _dllSupport.CreateInstance(CLSID_DxcCompiler, &dxcCompiler);

    CA2W shWide("lib_6_3", CP_UTF8);
    dxcCompiler->Compile(pSource,
        L"rayTracingUberShader.hlsl",
        L"",
        shWide,
        nullptr, 0, nullptr, 0, nullptr, &dxcResult);

    ComPtr<IDxcBlob> pResultBlob;
    dxcResult->GetResult(&pResultBlob);

    if (pResultBlob) {
        OutputDebugStringA((char*)pResultBlob->GetBufferPointer());
    }

    HRESULT result;
    dxcResult->GetStatus(&result);
    if (FAILED(result)) {
        CComPtr<IDxcBlobEncoding> pErr;
        dxcResult->GetErrorBuffer(&pErr);
        OutputDebugStringA((char*)pErr->GetBufferPointer());
    }

    CComPtr<IDxcBlob> pProgram;
    dxcResult->GetResult(&pProgram);
    /* if (opts.IsRootSignatureProfile()) {
         return;
     }*/

    CComPtr<IDxcBlobEncoding> pDisassembleBlob;
    dxcCompiler->Disassemble(pProgram, &pDisassembleBlob);

    std::string disassembleString((const char *)pDisassembleBlob->GetBufferPointer());
    OutputDebugStringA((char*)disassembleString.c_str());


    //// This contains the shaders and their entrypoints for the state object.
    //// Since shaders are not considered a subobject, they need to be passed in via DXIL library subobjects.
    auto lib = raytracingPipeline.CreateSubobject<CD3D12_DXIL_LIBRARY_SUBOBJECT>();
    D3D12_SHADER_BYTECODE libdxil = CD3DX12_SHADER_BYTECODE((unsigned char *)pProgram->GetBufferPointer(), pProgram->GetBufferSize());
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
    auto pipelineConfig = raytracingPipeline.CreateSubobject<CD3D12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
    // PERFOMANCE TIP: Set max recursion depth as low as needed 
    // as drivers may apply optimization strategies for low recursion depths. 
    UINT maxRecursionDepth = 1; // ~ primary rays only. 
    pipelineConfig->Config(maxRecursionDepth);

    _dxrDevice->CreateStateObject(raytracingPipeline, IID_PPV_ARGS(&_dxrStateObject));

    //DESCRIPTOR HEAP!!!!
    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
    // Allocate a heap for 5 descriptors:
    // 2 - vertex and index buffer SRVs
    // 1 - raytracing output texture SRV
    // 2 - bottom and top level acceleration structure fallback wrapped pointer UAVs
    descriptorHeapDesc.NumDescriptors = 5;
    descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    descriptorHeapDesc.NodeMask = 0;
    device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&_descriptorHeap));

    _descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    auto indexGPUAddress = (*entity->getFrustumVAO())[0]->getIndexResource()->getResource()->GetGPUVirtualAddress();
    auto vertexGPUAddress = (*entity->getFrustumVAO())[0]->getVertexResource()->getResource()->GetGPUVirtualAddress();

    auto indexDesc = (*entity->getFrustumVAO())[0]->getIndexResource()->getResource()->GetDesc();
    auto vertexDesc = (*entity->getFrustumVAO())[0]->getVertexResource()->getResource()->GetDesc();

    struct Vertex {
        float pos[3];
        float normal[3];
        float uv[2];
    };

    D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc = {};
    geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
    geometryDesc.Triangles.IndexBuffer = indexGPUAddress;
    geometryDesc.Triangles.IndexCount = static_cast<UINT>(indexDesc.Width) / sizeof(uint32_t);
    geometryDesc.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;
    geometryDesc.Triangles.Transform3x4 = 0;
    geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
    geometryDesc.Triangles.VertexCount = static_cast<UINT>(vertexDesc.Width) / sizeof(Vertex);
    geometryDesc.Triangles.VertexBuffer.StartAddress = vertexGPUAddress;
    geometryDesc.Triangles.VertexBuffer.StrideInBytes = sizeof(Vertex);

    // Mark the geometry as opaque. 
   // PERFORMANCE TIP: mark geometry as opaque whenever applicable as it can enable important ray processing optimizations.
   // Note: When rays encounter opaque geometry an any hit shader will not be executed whether it is present or not.
    geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

    // Get required sizes for an acceleration structure.
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottomLevelBuildDesc = {};
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS &bottomLevelInputs = bottomLevelBuildDesc.Inputs;
    bottomLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    bottomLevelInputs.Flags = buildFlags;
    bottomLevelInputs.NumDescs = 1;
    bottomLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
    bottomLevelInputs.pGeometryDescs = &geometryDesc;

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC topLevelBuildDesc = bottomLevelBuildDesc;
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS &topLevelInputs = topLevelBuildDesc.Inputs;
    topLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    topLevelInputs.Flags = buildFlags;
    topLevelInputs.NumDescs = 1;
    topLevelInputs.pGeometryDescs = nullptr;
    topLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO topLevelPrebuildInfo = {};

    _dxrDevice->GetRaytracingAccelerationStructurePrebuildInfo(&topLevelInputs, &topLevelPrebuildInfo);

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO bottomLevelPrebuildInfo = {};
    _dxrDevice->GetRaytracingAccelerationStructurePrebuildInfo(&bottomLevelInputs, &bottomLevelPrebuildInfo);

    ComPtr<ID3D12Resource> scratchResource;
    auto sizeBuildInfo = max(topLevelPrebuildInfo.ScratchDataSizeInBytes, bottomLevelPrebuildInfo.ScratchDataSizeInBytes);
    auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeBuildInfo, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    device->CreateCommittedResource(
        &uploadHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
        nullptr,
        IID_PPV_ARGS(&scratchResource));

    // Allocate resources for acceleration structures.
    // Acceleration structures can only be placed in resources that are created in the default heap (or custom heap equivalent). 
    // Default heap is OK since the application doesn’t need CPU read/write access to them. 
    // The resources that will contain acceleration structures must be created in the state D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, 
    // and must have resource flag D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS. The ALLOW_UNORDERED_ACCESS requirement simply acknowledges both: 
    //  - the system will be doing this type of access in its implementation of acceleration structure builds behind the scenes.
    //  - from the app point of view, synchronization of writes/reads to acceleration structures is accomplished using UAV barriers.
    {
        D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
        auto sizeBuildInfo = bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes;
        auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeBuildInfo, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
        device->CreateCommittedResource(
            &uploadHeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            initialResourceState,
            nullptr,
            IID_PPV_ARGS(&_bottomLevelAccelerationStructure));

    }

    {
        D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
        auto sizeBuildInfo = topLevelPrebuildInfo.ResultDataMaxSizeInBytes;
        auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeBuildInfo, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
        device->CreateCommittedResource(
            &uploadHeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            initialResourceState,
            nullptr,
            IID_PPV_ARGS(&_topLevelAccelerationStructure));
    }
    // Create an instance desc for the bottom-level acceleration structure.
    ComPtr<ID3D12Resource> instanceDescs;
    D3D12_RAYTRACING_INSTANCE_DESC instanceDesc = {};
    instanceDesc.Transform[0][0] = instanceDesc.Transform[1][1] = instanceDesc.Transform[2][2] = 1;
    instanceDesc.InstanceMask = 1;
    instanceDesc.AccelerationStructure = _bottomLevelAccelerationStructure->GetGPUVirtualAddress();
    {
        D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
        auto sizeBuildInfo = sizeof(instanceDesc);
        auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeBuildInfo, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
        device->CreateCommittedResource(
            &uploadHeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&instanceDescs));

    }

    // Bottom Level Acceleration Structure desc
    {
        bottomLevelBuildDesc.ScratchAccelerationStructureData = scratchResource->GetGPUVirtualAddress();
        bottomLevelBuildDesc.DestAccelerationStructureData = _bottomLevelAccelerationStructure->GetGPUVirtualAddress();
    }

    // Top Level Acceleration Structure desc
    {
        topLevelBuildDesc.DestAccelerationStructureData = _topLevelAccelerationStructure->GetGPUVirtualAddress();
        topLevelBuildDesc.ScratchAccelerationStructureData = scratchResource->GetGPUVirtualAddress();
        topLevelBuildDesc.Inputs.InstanceDescs = instanceDescs->GetGPUVirtualAddress();
    }

    auto BuildAccelerationStructure = [&](auto* raytracingCommandList)
    {
        raytracingCommandList->BuildRaytracingAccelerationStructure(&bottomLevelBuildDesc, 0, nullptr);
        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::UAV(_bottomLevelAccelerationStructure.Get()));
        raytracingCommandList->BuildRaytracingAccelerationStructure(&topLevelBuildDesc, 0, nullptr);
    };

    BuildAccelerationStructure(_dxrCommandList.Get());

    commandList->Close();
    ID3D12CommandList *commandLists[] = { commandList.Get() };
    auto cmdQueue = DXLayer::instance()->getCmdQueue();
    cmdQueue->ExecuteCommandLists(ARRAYSIZE(commandLists), commandLists);

    DXLayer::instance()->fenceCommandList();

    // Allocate one constant buffer per frame, since it gets updated every frame.
    size_t cbSize = 2 * sizeof(AlignedSceneConstantBuffer);
    const D3D12_RESOURCE_DESC constantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(cbSize);
    auto uploadHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

   device->CreateCommittedResource(
        &uploadHeapProps,
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
        rayGenShaderIdentifier = stateObjectProperties->GetShaderIdentifier(c_raygenShaderName);
        missShaderIdentifier = stateObjectProperties->GetShaderIdentifier(c_missShaderName);
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
        ShaderTable rayGenShaderTable(device.Get(), numShaderRecords, shaderRecordSize, L"RayGenShaderTable");
        rayGenShaderTable.push_back(ShaderRecord(rayGenShaderIdentifier, shaderIdentifierSize));
        _rayGenShaderTable = rayGenShaderTable.GetResource();
    }

    // Miss shader table
    {
        UINT numShaderRecords = 1;
        UINT shaderRecordSize = shaderIdentifierSize;
        ShaderTable missShaderTable(device.Get(), numShaderRecords, shaderRecordSize, L"MissShaderTable");
        missShaderTable.push_back(ShaderRecord(missShaderIdentifier, shaderIdentifierSize));
        _missShaderTable = missShaderTable.GetResource();
    }

    // Hit group shader table
    {
        struct RootArguments {
            CubeConstantBuffer cb;
        } rootArguments;
        rootArguments.cb = _cubeCB;

        UINT numShaderRecords = 1;
        UINT shaderRecordSize = shaderIdentifierSize + sizeof(rootArguments);
        ShaderTable hitGroupShaderTable(device.Get(), numShaderRecords, shaderRecordSize, L"HitGroupShaderTable");
        hitGroupShaderTable.push_back(ShaderRecord(hitGroupShaderIdentifier, shaderIdentifierSize, &rootArguments, sizeof(rootArguments)));
        _hitGroupShaderTable = hitGroupShaderTable.GetResource();
    }

    // Create the output resource. The dimensions and format should match the swap-chain.
    auto uavDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, 1920, 1080, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

    auto defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    device->CreateCommittedResource(
        &defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &uavDesc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, IID_PPV_ARGS(&_raytracingOutput));

    _raytracingOutputResourceUAVDescriptorHeapIndex = 1;
    D3D12_CPU_DESCRIPTOR_HANDLE uavDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
        _descriptorHeap->GetCPUDescriptorHandleForHeapStart(),
        _raytracingOutputResourceUAVDescriptorHeapIndex,
        _descriptorSize);

    D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
    UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    device->CreateUnorderedAccessView(_raytracingOutput.Get(), nullptr, &UAVDesc, uavDescriptorHandle);
    
    _raytracingOutputResourceUAVGpuDescriptor = 
        CD3DX12_GPU_DESCRIPTOR_HANDLE(_descriptorHeap->GetGPUDescriptorHandleForHeapStart(), 
            _raytracingOutputResourceUAVDescriptorHeapIndex, 
            _descriptorSize);

    DXLayer::instance()->initCmdLists();
}

void RayTracingPipelineShader::_queryShaderResources(ComPtr<ID3DBlob> shaderBlob) {
    
    ID3D12ShaderReflection* reflectionInterface;
    D3DReflect(shaderBlob->GetBufferPointer(), 
        shaderBlob->GetBufferSize(), 
        IID_ID3D12ShaderReflection, 
        (void**)&reflectionInterface);

    HRESULT result = S_OK;

    PipelineShader::_queryShaderResources(shaderBlob);
    
    //Now do raytracing specific shit
}

void RayTracingPipelineShader::DoRayTracing(Entity* entity)
{
    auto commandList = DXLayer::instance()->getCmdList();
    auto frameIndex = DXLayer::instance()->getCmdListIndex();

    auto DispatchRays = [&](auto* commandList, auto* stateObject, auto* dispatchDesc)
    {
        // Since each shader table has only one shader record, the stride is same as the size.
        dispatchDesc->HitGroupTable.StartAddress = _hitGroupShaderTable->GetGPUVirtualAddress();
        dispatchDesc->HitGroupTable.SizeInBytes = _hitGroupShaderTable->GetDesc().Width;
        dispatchDesc->HitGroupTable.StrideInBytes = dispatchDesc->HitGroupTable.SizeInBytes;
        dispatchDesc->MissShaderTable.StartAddress = _missShaderTable->GetGPUVirtualAddress();
        dispatchDesc->MissShaderTable.SizeInBytes = _missShaderTable->GetDesc().Width;
        dispatchDesc->MissShaderTable.StrideInBytes = dispatchDesc->MissShaderTable.SizeInBytes;
        dispatchDesc->RayGenerationShaderRecord.StartAddress = _rayGenShaderTable->GetGPUVirtualAddress();
        dispatchDesc->RayGenerationShaderRecord.SizeInBytes = _rayGenShaderTable->GetDesc().Width;
        dispatchDesc->Width = 1920;
        dispatchDesc->Height = 1080;
        dispatchDesc->Depth = 1;
        commandList->SetPipelineState1(stateObject);
        commandList->DispatchRays(dispatchDesc);
    };

    auto SetCommonPipelineState = [&](ComPtr<ID3D12GraphicsCommandList> descriptorSetCommandList)
    {

        auto gpuHandle = 
            CD3DX12_GPU_DESCRIPTOR_HANDLE(_descriptorHeap->GetGPUDescriptorHandleForHeapStart(), 0, _descriptorSize);

        descriptorSetCommandList->SetDescriptorHeaps(1, _descriptorHeap.GetAddressOf());
        // Set index and successive vertex buffer decriptor tables
        commandList->SetComputeRootDescriptorTable(GlobalRootSignatureParams::VertexBuffersSlot, gpuHandle);
        commandList->SetComputeRootDescriptorTable(GlobalRootSignatureParams::OutputViewSlot, _raytracingOutputResourceUAVGpuDescriptor);
    };

    commandList->SetComputeRootSignature(_raytracingGlobalRootSignature.Get());

    // Copy the updated scene constant buffer to GPU.
    memcpy(&_mappedConstantData[frameIndex].constants, &_sceneCB[frameIndex], sizeof(_sceneCB[frameIndex]));
    auto cbGpuAddress = _perFrameConstants->GetGPUVirtualAddress() + frameIndex * sizeof(_mappedConstantData[0]);
    commandList->SetComputeRootConstantBufferView(GlobalRootSignatureParams::SceneConstantSlot, cbGpuAddress);

    // Bind the heaps, acceleration structure and dispatch rays.
    D3D12_DISPATCH_RAYS_DESC dispatchDesc = {};
   
    SetCommonPipelineState(commandList);
    commandList->SetComputeRootShaderResourceView(GlobalRootSignatureParams::AccelerationStructureSlot, _topLevelAccelerationStructure->GetGPUVirtualAddress());
    
    DispatchRays(_dxrCommandList.Get(), _dxrStateObject.Get(), &dispatchDesc);
    
}