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

    if(_textures.find(textureName) == _textures.end()){
        _textures[textureName] = new Texture(textureName);
    }
}

Texture* TextureBroker::getTexture(std::string textureName) {
    return _textures[textureName];
}