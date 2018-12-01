#include "RayTracingPipelineShader.h"
#include "DXLayer.h"
#include "D3D12RaytracingHelpers.hpp"
#include "EngineManager.h"

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
    DXGI_FORMAT format) {

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
