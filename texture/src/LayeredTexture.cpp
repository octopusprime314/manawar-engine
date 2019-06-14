#include "LayeredTexture.h"
#include "AssetTexture.h"

LayeredTexture::LayeredTexture(std::vector<AssetTexture*> textures) {
    _textures = textures;
}

LayeredTexture::LayeredTexture(std::vector<AssetTexture*> textures,
    ComPtr<ID3D12GraphicsCommandList>& cmdList,
    ComPtr<ID3D12Device>& device) {
    _textures = textures;
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

void LayeredTexture::updateTexture(std::string textureName, FIBITMAP* pixelData) {

    for (int i = 0; i < _textures.size(); i++) {
        auto textureLayer = _textures[i];
        if (textureLayer->getName().find(textureName) != std::string::npos) {
            _textures[i]->updateTexture(pixelData);
        }
    }
}

