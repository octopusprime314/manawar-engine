#include "Texture.h"

Texture::Texture() {

}

Texture::Texture(std::string name) :
    _name(name) {
    
}

Texture::Texture(std::string name, GLuint width, GLuint height) :
    _name(name),
    _width(width),
    _height(height) {

}

Texture::~Texture() {

}

GLuint Texture::getContext() {
    return _textureContext;
}

GLuint Texture::getWidth() {
    return _width;
}

GLuint Texture::getHeight() {
    return _height;
}

std::string Texture::getName() {
    return _name;
}

ResourceBuffer* Texture::getResource() {
    return _textureBuffer;
}

void Texture::bindToDXShader(ComPtr<ID3D12GraphicsCommandList>& cmdList, 
    UINT textureBinding,
    std::map<std::string, UINT>& resourceBindings) {

    if (_srvDescriptorHeap != nullptr && _samplerDescriptorHeap != nullptr &&
        resourceBindings.find("textureSampler") != resourceBindings.end()) {
        
        ID3D12DescriptorHeap* descriptorHeaps[] = { _srvDescriptorHeap.Get(),  _samplerDescriptorHeap.Get() };
        cmdList->SetDescriptorHeaps(2, descriptorHeaps);

        cmdList->SetGraphicsRootDescriptorTable(textureBinding,
            _srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

        cmdList->SetGraphicsRootDescriptorTable(resourceBindings["textureSampler"],
            _samplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
        
    }
    //No sampler and probably just a compute shader
    else {
        ID3D12DescriptorHeap* descriptorHeaps[] = { _srvDescriptorHeap.Get() };
        cmdList->SetDescriptorHeaps(1, descriptorHeaps);

        cmdList->SetComputeRootDescriptorTable(textureBinding,
            _srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
    }
}
