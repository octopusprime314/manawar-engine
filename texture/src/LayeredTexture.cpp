#include "LayeredTexture.h"

LayeredTexture::LayeredTexture(std::vector<std::string> textureNames) {

    for (auto textureName : textureNames) {
        _textures.push_back(new AssetTexture(textureName));
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
