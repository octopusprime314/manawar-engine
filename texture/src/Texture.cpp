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
