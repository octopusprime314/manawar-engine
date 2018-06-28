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
