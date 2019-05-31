#include "LayeredTexture.h"

LayeredTexture::LayeredTexture(std::vector<std::string> textureNames) {

    for (auto textureName : textureNames) {
        _textures.push_back(new AssetTexture(textureName));
    }
}

LayeredTexture::LayeredTexture(std::vector<std::string> textureNames,
    ComPtr<ID3D12GraphicsCommandList>& cmdList,
    ComPtr<ID3D12Device>& device) {
    
    for (auto textureName : textureNames) {
        _textures.push_back(new AssetTexture(textureName, cmdList, device));
    }
}

LayeredTexture::~LayeredTexture() {

}

std::vector<AssetTexture*> LayeredTexture::getTextures() {
    return _textures;
}

void LayeredTexture::setTexture(AssetTexture* texture) {
    for (int i = 0; i < _textures.size(); i++) {
        auto textureLayer = _textures[i];
        if (textureLayer->getName().find(texture->getName()) != std::string::npos) {
            _textures[i] = new AssetTexture(textureLayer->getName());
        }
    }
}

void LayeredTexture::updateTexture(std::string textureName, void* pixelData) {
    
    for (int i = 0; i < _textures.size(); i++) {
        auto textureLayer = _textures[i];
        if (textureLayer->getName().find(textureName) != std::string::npos) {
            _textures[i]->updateTexture(pixelData);
        }
    }
}

