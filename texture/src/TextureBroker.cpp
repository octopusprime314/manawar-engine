#include "TextureBroker.h"
TextureBroker* TextureBroker::_broker = nullptr;

TextureBroker* TextureBroker::instance() { //Only initializes the static pointer once
    if (_broker == nullptr) {
        _broker = new TextureBroker();
    }
    return _broker;
}
TextureBroker::TextureBroker() {

}
TextureBroker::~TextureBroker() {

}

void TextureBroker::addTexture(std::string textureName) {

    if (_textures.find(textureName) == _textures.end()) {
        _textures[textureName] = new AssetTexture(textureName);
    }
}

void TextureBroker::addLayeredTexture(std::vector<std::string> textureNames) {

    std::string sumString = "Layered";
    for (auto& str : textureNames) {
        sumString += str;
    }
    if (_layeredTextures.find(sumString) == _layeredTextures.end()) {
        _layeredTextures[sumString] = new LayeredTexture(textureNames);
    }
}

void TextureBroker::addCubeTexture(std::string textureName) {
    if (_textures.find(textureName) == _textures.end()) {
        _textures[textureName] = new AssetTexture(textureName, true);
    }
}

AssetTexture* TextureBroker::getTexture(std::string textureName) {
    return _textures[textureName];
}

LayeredTexture* TextureBroker::getLayeredTexture(std::string textureName) {
    return _layeredTextures[textureName];
}
