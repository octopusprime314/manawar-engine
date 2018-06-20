#include "LayeredTexture.h"

LayeredTexture::LayeredTexture(std::vector<std::string> textureNames) {

    for (auto textureName : textureNames) {
        _textures.push_back(new Texture(textureName));
    }
}

LayeredTexture::~LayeredTexture() {

}

std::vector<Texture*> LayeredTexture::getTextures() {
    return _textures;
}
