#include "AssetTexture.h"

AssetTexture::AssetTexture() {

}

AssetTexture::AssetTexture(std::string textureName, bool cubeMap) :
    Texture(textureName),
    _alphaValues(false) {

    if (!cubeMap) {
        if (_getTextureData(_name)) {
            _build2DTextureGL(_name);
        }
    }
    else {
        _buildCubeMapTextureGL(_name);
    }
}

AssetTexture::AssetTexture(std::string textureName,
                           ComPtr<ID3D12GraphicsCommandList>& cmdList,
                           ComPtr<ID3D12Device>& device,
                           bool cubeMap) :
    Texture(textureName),
    _alphaValues(false) {

    if (!cubeMap) {
        if (_getTextureData(_name)) {

            //Don't count as an alpha texture if all values are set to 1.0
            if (_bits[3] == 255) {
                _alphaValues = false;
            }
            _build2DTextureDX(_name, cmdList, device);
            buildMipLevels();
        }
    }
    else {
        
        _buildCubeMapTextureDX(_name, cmdList, device);
        //buildMipLevels();
    }
}

AssetTexture::AssetTexture(void* data, UINT width, UINT height,
    ComPtr<ID3D12GraphicsCommandList>& cmdList,
    ComPtr<ID3D12Device>& device) :
    Texture(""),
    _alphaValues(false) {

    _build2DTextureDX(data, width, height, cmdList, device);
    buildMipLevels();
}

AssetTexture::AssetTexture(void* data, UINT width, UINT height) :
    Texture(""),
    _alphaValues(false) {

    _build2DTextureGL(data, width, height);
}

AssetTexture::~AssetTexture() {

}

void AssetTexture::updateTexture(void* data) {
    memcpy(_bits, data, _sizeInBytes);
    _build2DTextureGL(_name);
}

void AssetTexture::_build2DTextureGL(void* data, UINT width, UINT height) {
    
    glGenTextures(1, &_textureContext);
    glBindTexture(GL_TEXTURE_2D, _textureContext);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void AssetTexture::_build2DTextureGL(std::string textureName) {

    //Generate a texture context
    glGenTextures(1, &_textureContext);

    //Bind the generated reference context to load texture data
    glBindTexture(GL_TEXTURE_2D, _textureContext);

    _decodeTexture(textureName, GL_TEXTURE_2D);

    //if an alpha map is loaded then we clamp to edge for proper rgba channel interpolation
    if (textureName.find("alphamap") != std::string::npos) {

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    //Enable anisotropic filtering which prevent bluring of low res mip map textures
    //Preserves high texture resolution regardless
    float aniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &aniso);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, aniso);

    //Free FreeImage's copy of the data
    //FreeImage_Unload(_dib);
}

void AssetTexture::_build2DTextureDX(void* data, UINT width, UINT height, 
                                    ComPtr<ID3D12GraphicsCommandList>& cmdList,
                                    ComPtr<ID3D12Device>& device) {

    _textureBuffer = new ResourceBuffer(data, width * height * 4, width, height, width * 4, cmdList, device);

    //Create descriptor heap
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc;
    ZeroMemory(&srvHeapDesc, sizeof(srvHeapDesc));
    srvHeapDesc.NumDescriptors = 1; //1 2D texture
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(_srvDescriptorHeap.GetAddressOf()));

    //Create view of SRV for shader access
    CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(_srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    auto textureDescriptor = _textureBuffer->getDescriptor();
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = textureDescriptor.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = textureDescriptor.MipLevels;
    srvDesc.Texture2D.ResourceMinLODClamp = 0;
    device->CreateShaderResourceView(_textureBuffer->getResource().Get(), &srvDesc, hDescriptor);

    // create sampler descriptor heap
    D3D12_DESCRIPTOR_HEAP_DESC descHeapSampler = {};
    descHeapSampler.NumDescriptors = 1;
    descHeapSampler.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
    descHeapSampler.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    device->CreateDescriptorHeap(&descHeapSampler,
        IID_PPV_ARGS(_samplerDescriptorHeap.GetAddressOf()));

    // create sampler descriptor in the sample descriptor heap
    D3D12_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(D3D12_SAMPLER_DESC));
    samplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;// D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    device->CreateSampler(&samplerDesc,
        _samplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

void AssetTexture::_build2DTextureDX(std::string textureName,
                                   ComPtr<ID3D12GraphicsCommandList>& cmdList,
                                   ComPtr<ID3D12Device>& device) {

    auto rowPitch = FreeImage_GetPitch(_dib);
    _textureBuffer = new ResourceBuffer(_bits, _imageBufferSize, _width, _height, rowPitch, cmdList, device);

    //Create descriptor heap
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc;
    ZeroMemory(&srvHeapDesc, sizeof(srvHeapDesc));
    srvHeapDesc.NumDescriptors = 1; //1 2D texture
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(_srvDescriptorHeap.GetAddressOf()));

    //Create view of SRV for shader access
    CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(_srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    auto textureDescriptor = _textureBuffer->getDescriptor();
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = textureDescriptor.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = textureDescriptor.MipLevels;
    srvDesc.Texture2D.ResourceMinLODClamp = 0;
    device->CreateShaderResourceView(_textureBuffer->getResource().Get(), &srvDesc, hDescriptor);

    // create sampler descriptor heap
    D3D12_DESCRIPTOR_HEAP_DESC descHeapSampler = {};
    descHeapSampler.NumDescriptors = 1;
    descHeapSampler.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
    descHeapSampler.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    device->CreateDescriptorHeap(&descHeapSampler,
        IID_PPV_ARGS(_samplerDescriptorHeap.GetAddressOf()));

    // create sampler descriptor in the sample descriptor heap
    D3D12_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(D3D12_SAMPLER_DESC));
    samplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;// D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    device->CreateSampler(&samplerDesc,
        _samplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

void AssetTexture::buildMipLevels() {
    _textureBuffer->buildMipLevels(this);
}

BYTE* AssetTexture::getBits() {
    return _bits;
}

void AssetTexture::_buildCubeMapTextureGL(std::string skyboxName) {
    glGenTextures(1, &_textureContext);

    //Bind the texture and create 6 sides of a texture cube
    glBindTexture(GL_TEXTURE_CUBE_MAP, _textureContext);
    for (unsigned int i = 0; i < 6; ++i) {
        std::string textureName = "";
        if (i == 0) {
            textureName = skyboxName + "/front.jpg";
            _getTextureData(textureName);
        }
        else  if (i == 1) {
            textureName = skyboxName + "/back.jpg";
            _getTextureData(textureName);
        }
        else if (i == 2) {
            textureName = skyboxName + "/top.jpg";
            _getTextureData(textureName);
        }
        else if (i == 3) {
            textureName = skyboxName + "/bottom.jpg";
            _getTextureData(textureName);
        }
        else if (i == 4) {
            textureName = skyboxName + "/right.jpg";
            _getTextureData(textureName);
        }
        else if (i == 5) {
            textureName = skyboxName + "/left.jpg";
            _getTextureData(textureName);
        }

        _decodeTexture(textureName, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);

        //Free FreeImage's copy of the data
        FreeImage_Unload(_dib);
    }

    //Texture params
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void AssetTexture::_buildCubeMapTextureDX(std::string skyboxName,
                                          ComPtr<ID3D12GraphicsCommandList>& cmdList,
                                          ComPtr<ID3D12Device>& device) {

    std::vector<unsigned char> cubeMapData;
    unsigned int rowPitch = 0;
    for (unsigned int i = 0; i < 6; ++i) {
        std::string textureName = "";
        if (i == 0) {
            textureName = skyboxName + "/front.jpg";
            _getTextureData(textureName);
        }
        else  if (i == 1) {
            textureName = skyboxName + "/back.jpg";
            _getTextureData(textureName);
        }
        else if (i == 2) {
            textureName = skyboxName + "/top.jpg";
            _getTextureData(textureName);
        }
        else if (i == 3) {
            textureName = skyboxName + "/bottom.jpg";
            _getTextureData(textureName);
        }
        else if (i == 4) {
            textureName = skyboxName + "/right.jpg";
            _getTextureData(textureName);
        }
        else if (i == 5) {
            textureName = skyboxName + "/left.jpg";
            _getTextureData(textureName);
        }
        //Don't count as an alpha texture if all values are set to 1.0
        if (_bits[3] == 255) {
            _alphaValues = false;
        }

        //_build2DTextureDX(textureName, cmdList, device);
        rowPitch = FreeImage_GetPitch(_dib);
        auto size = FreeImage_GetMemorySize(_dib);
        cubeMapData.insert(cubeMapData.end(), &_bits[0], &_bits[rowPitch * _height]);

        //Free FreeImage's copy of the data
        FreeImage_Unload(_dib);
    }

    _textureBuffer = new ResourceBuffer(cubeMapData.data(), 6, 
        static_cast<UINT>(cubeMapData.size()), _width, _height, rowPitch, cmdList, device);

    //Create descriptor heap
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc;
    ZeroMemory(&srvHeapDesc, sizeof(srvHeapDesc));
    srvHeapDesc.NumDescriptors = 1; //1 Cubemap texture
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(_srvDescriptorHeap.GetAddressOf()));

    //Create view of SRV for shader access
    CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(_srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    auto textureDescriptor = _textureBuffer->getDescriptor();
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = textureDescriptor.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.TextureCube.MostDetailedMip = 0;
    srvDesc.TextureCube.MipLevels = textureDescriptor.MipLevels;
    srvDesc.TextureCube.ResourceMinLODClamp = 0;
    device->CreateShaderResourceView(_textureBuffer->getResource().Get(), &srvDesc, hDescriptor);

    // create sampler descriptor heap
    D3D12_DESCRIPTOR_HEAP_DESC descHeapSampler = {};
    descHeapSampler.NumDescriptors = 1;
    descHeapSampler.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
    descHeapSampler.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    device->CreateDescriptorHeap(&descHeapSampler,
        IID_PPV_ARGS(_samplerDescriptorHeap.GetAddressOf()));

    // create sampler descriptor in the sample descriptor heap
    D3D12_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(D3D12_SAMPLER_DESC));
    samplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;// D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    device->CreateSampler(&samplerDesc,
        _samplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
   
}

bool AssetTexture::_getTextureData(std::string textureName) {
    //image format
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

    //check the file signature and deduce its format
    fif = FreeImage_GetFileType(textureName.c_str(), 0);

    //if still unknown, try to guess the file format from the file extension
    if (fif == FIF_UNKNOWN) {
        fif = FreeImage_GetFIFFromFilename(textureName.c_str());
    }
    //if still unkown, return failure
    if (fif == FIF_UNKNOWN) {
        return false;
    }

    //check that the plugin has reading capabilities and load the file
    if (FreeImage_FIFSupportsReading(fif)) {
        _dib = FreeImage_Load(fif, textureName.c_str());
    }
    //if the image failed to load, return failure
    if (!_dib) {
        return false;
    }

    //retrieve the image data
    _bits = FreeImage_GetBits(_dib);
    //get the image width and height
    _width = FreeImage_GetWidth(_dib);
    _height = FreeImage_GetHeight(_dib);
    //if this somehow one of these failed (they shouldn't), return failure
    if ((_bits == 0) || (_width == 0) || (_height == 0)) {
        return false;
    }

    _sizeInBytes = FreeImage_GetMemorySize(_dib);
    _imageBufferSize = _width * _height * 4;

    if (_sizeInBytes >= _imageBufferSize) {
        _alphaValues = true;
    }
    else {
        _imageBufferSize = _width * _height * 3;
    }

    return true;
}

void AssetTexture::_decodeTexture(std::string textureName, unsigned int textureType) {
    glCheck();
    //Use mip maps to prevent anti aliasing issues
    if (textureName.substr(textureName.find_last_of('.')) == ".tif" && _alphaValues) {
        glTexImage2D(textureType, 0, GL_RGBA8, _width, _height, 0, GL_BGRA, GL_UNSIGNED_BYTE, _bits);
        glGenerateMipmap(textureType); //Allocate mipmaps
        //If alpha value is opaque then do not count as a transparent texture
        if (_bits[3] == 255) {
            _alphaValues = false;
        }
    }
    else if (textureName.substr(textureName.find_last_of('.')) == ".tif" && !_alphaValues) {
        glTexImage2D(textureType, 0, GL_RGB8, _width, _height, 0, GL_BGR, GL_UNSIGNED_BYTE, _bits);
        glGenerateMipmap(textureType); //Allocate mipmaps
    }
    else if (textureName.substr(textureName.find_last_of('.')) == ".dds" && _alphaValues) {
        glTexImage2D(textureType, 0, GL_RGBA8, _width, _height, 0, GL_BGRA, GL_UNSIGNED_BYTE, _bits);
        glGenerateMipmap(textureType); //Allocate mipmaps
        //If alpha value is opaque then do not count as a transparent texture
        if (_bits[3] == 255) {
            _alphaValues = false;
        }
    }
    else if (textureName.substr(textureName.find_last_of('.')) == ".dds" && !_alphaValues) {
        glTexImage2D(textureType, 0, GL_RGB8, _width, _height, 0, GL_BGR, GL_UNSIGNED_BYTE, _bits);
        glGenerateMipmap(textureType); //Allocate mipmaps
    }
    else if (textureName.substr(textureName.find_last_of('.')) == ".tga" && _alphaValues) {
        glTexImage2D(textureType, 0, GL_RGBA8, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _bits);
        glGenerateMipmap(textureType); //Allocate mipmaps
    }
    else if (textureName.substr(textureName.find_last_of('.')) == ".tga" && !_alphaValues) {
        glTexImage2D(textureType, 0, GL_RGB8, _width, _height, 0, GL_BGR, GL_UNSIGNED_BYTE, _bits);
        glGenerateMipmap(textureType); //Allocate mipmaps
    }
    else if (textureName.substr(textureName.find_last_of('.')) == ".jpg" && _alphaValues) {
        glTexImage2D(textureType, 0, GL_RGBA8, _width, _height, 0, GL_BGRA, GL_UNSIGNED_BYTE, _bits);
        glGenerateMipmap(textureType); //Allocate mipmaps
    }
    else if (textureName.substr(textureName.find_last_of('.')) == ".jpg" && !_alphaValues) {
        glTexImage2D(textureType, 0, GL_RGB8, _width, _height, 0, GL_BGR, GL_UNSIGNED_BYTE, _bits);
        glGenerateMipmap(textureType); //Allocate mipmaps
    }
    else if (textureName.substr(textureName.find_last_of('.')) == ".bmp" && _alphaValues) {
        glTexImage2D(textureType, 0, GL_RGBA8, _width, _height, 0, GL_BGRA, GL_UNSIGNED_BYTE, _bits);
        glGenerateMipmap(textureType); //Allocate mipmaps
    }
    else if (textureName.substr(textureName.find_last_of('.')) == ".bmp" && !_alphaValues) {
        glTexImage2D(textureType, 0, GL_RGB8, _width, _height, 0, GL_BGR, GL_UNSIGNED_BYTE, _bits);
        glGenerateMipmap(textureType); //Allocate mipmaps
    }
    else if (textureName.substr(textureName.find_last_of('.')) == ".png" && _alphaValues) {
        glTexImage2D(textureType, 0, GL_RGBA8, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _bits);
        glGenerateMipmap(textureType); //Allocate mipmaps
    }
    else if (textureName.substr(textureName.find_last_of('.')) == ".png" && !_alphaValues) {
        glTexImage2D(textureType, 0, GL_RGB8, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, _bits);
        glGenerateMipmap(textureType); //Allocate mipmaps
    }
    // glGenerateMipmap() above frequently generates an error. Instead of dealing with it, ignore it.
    if (glGetError() != GL_NO_ERROR) {
        printf("%s:%d %s(): Something involving mipmaps generated an error, and we are going to ignore it.\n",
            __FILE__, __LINE__, __func__);
    }
    // Now this call will "pass"
    glCheck();
}

bool AssetTexture::getTransparency() {
    return _alphaValues;
}