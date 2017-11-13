#include "TextureBroker.h"

TextureBroker::TextureBroker() {

}
    
TextureBroker::~TextureBroker() {

}

void TextureBroker::addTexture(Texture* texture) {
    _textures.push_back(texture);
}

Texture* TextureBroker::getTexture(int index) {
    return _textures[index];
}