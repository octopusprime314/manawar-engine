#include "Texture.h"

Texture::Texture(){

}

Texture::Texture(std::string textureName, bool cubeMap){

    if (!cubeMap) {
        if (_getTextureData(textureName)) {
            _build2DTexture(textureName);
        }
    }
    else {

        _buildCubeMapTexture(textureName);
    }
}

Texture::~Texture(){

}

GLuint Texture::getContext(){
    return _textureContext;
}

void Texture::_build2DTexture(std::string textureName) {
    
    //Generate a texture context
    glGenTextures(1, &_textureContext);

    //Bind the generated reference context to load texture data
    glBindTexture(GL_TEXTURE_2D, _textureContext);
    
    //Use mip maps to prevent anti aliasing issues
    if (textureName.substr(textureName.find_last_of('.')) == ".tif" && _alphaValues) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_BGRA, GL_UNSIGNED_BYTE, _bits);
        glGenerateMipmap(GL_TEXTURE_2D); //Allocate mipmaps 
    }
    else if (textureName.substr(textureName.find_last_of('.')) == ".tif" && !_alphaValues) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, _width, _height, 0, GL_BGR, GL_UNSIGNED_BYTE, _bits);
        glGenerateMipmap(GL_TEXTURE_2D); //Allocate mipmaps
    }
    else if (textureName.substr(textureName.find_last_of('.')) == ".dds" && _alphaValues) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_BGRA, GL_UNSIGNED_BYTE, _bits);
        glGenerateMipmap(GL_TEXTURE_2D); //Allocate mipmaps
    }
    else if (textureName.substr(textureName.find_last_of('.')) == ".dds" && !_alphaValues) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, _width, _height, 0, GL_BGR, GL_UNSIGNED_BYTE, _bits);
        glGenerateMipmap(GL_TEXTURE_2D); //Allocate mipmaps
    }
    else if (textureName.substr(textureName.find_last_of('.')) == ".tga" && _alphaValues) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_BGRA, GL_UNSIGNED_BYTE, _bits);
        glGenerateMipmap(GL_TEXTURE_2D); //Allocate mipmaps
    }
    else if (textureName.substr(textureName.find_last_of('.')) == ".tga" && !_alphaValues) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, _width, _height, 0, GL_BGR, GL_UNSIGNED_BYTE, _bits);
        glGenerateMipmap(GL_TEXTURE_2D); //Allocate mipmaps
    }
    else if (textureName.substr(textureName.find_last_of('.')) == ".jpg" && _alphaValues) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_BGRA, GL_UNSIGNED_BYTE, _bits);
        glGenerateMipmap(GL_TEXTURE_2D); //Allocate mipmaps
    }
    else if (textureName.substr(textureName.find_last_of('.')) == ".jpg" && !_alphaValues) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, _width, _height, 0, GL_BGR, GL_UNSIGNED_BYTE, _bits);
        glGenerateMipmap(GL_TEXTURE_2D); //Allocate mipmaps
    }
    else if (textureName.substr(textureName.find_last_of('.')) == ".bmp" && _alphaValues) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_BGRA, GL_UNSIGNED_BYTE, _bits);
        glGenerateMipmap(GL_TEXTURE_2D); //Allocate mipmaps
    }
    else if (textureName.substr(textureName.find_last_of('.')) == ".bmp" && !_alphaValues) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, _width, _height, 0, GL_BGR, GL_UNSIGNED_BYTE, _bits);
        glGenerateMipmap(GL_TEXTURE_2D); //Allocate mipmaps
    }
    else if (textureName.substr(textureName.find_last_of('.')) == ".png" && _alphaValues) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _bits);
        glGenerateMipmap(GL_TEXTURE_2D); //Allocate mipmaps
    }
    else if (textureName.substr(textureName.find_last_of('.')) == ".png" && !_alphaValues) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, _bits);
        glGenerateMipmap(GL_TEXTURE_2D); //Allocate mipmaps
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    //Enable anisotropic filtering which prevent bluring of low res mip map textures
    //Preserves high texture resolution regardless
    float aniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &aniso);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, aniso);

    //Free FreeImage's copy of the data
    FreeImage_Unload(_dib);
}

void Texture::_buildCubeMapTexture(std::string skyboxName) {
    
    glGenTextures(1, &_textureContext);

    //Bind the texture and create 6 sides of a texture cube
    glBindTexture(GL_TEXTURE_CUBE_MAP, _textureContext);
    for (unsigned int i = 0; i < 6; ++i) {
        if (i == 0) {
            _getTextureData(skyboxName + "/top.tga");
        }
        else  if (i == 1) {
            _getTextureData(skyboxName + "/bottom.tga");
        }
        else if (i == 2) {
            _getTextureData(skyboxName + "/right.tga");
        }
        else if (i == 3) {
            _getTextureData(skyboxName + "/left.tga");
        }
        else if (i == 4) {
            _getTextureData(skyboxName + "/front.tga");
        }
        else if (i == 5) {
            _getTextureData(skyboxName + "/back.tga");
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, _width, _height,
            0, GL_RGB, GL_UNSIGNED_BYTE, _bits);

        //Free FreeImage's copy of the data
        FreeImage_Unload(_dib);
    }

    //Texture params
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

bool Texture::_getTextureData(std::string textureName) {
    //image format
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

    //check the file signature and deduce its format
    fif = FreeImage_GetFileType(textureName.c_str(), 0);

    //if still unknown, try to guess the file format from the file extension
    if (fif == FIF_UNKNOWN) {
        fif = FreeImage_GetFIFFromFilename(textureName.c_str());
    }
    //if still unkown, return failure
    if (fif == FIF_UNKNOWN) {
        return false;
    }

    //check that the plugin has reading capabilities and load the file
    if (FreeImage_FIFSupportsReading(fif)) {
        _dib = FreeImage_Load(fif, textureName.c_str());
    }
    //if the image failed to load, return failure
    if (!_dib) {
        return false;
    }

    //retrieve the image data
    _bits = FreeImage_GetBits(_dib);
    //get the image width and height
    _width = FreeImage_GetWidth(_dib);
    _height = FreeImage_GetHeight(_dib);
    //if this somehow one of these failed (they shouldn't), return failure
    if ((_bits == 0) || (_width == 0) || (_height == 0)) {
        return false;
    }

    unsigned int imageSize = FreeImage_GetMemorySize(_dib);
    unsigned int byteSize = _width * _height * 4;

    _alphaValues = true;
    if (byteSize > imageSize) {
        _alphaValues = false;
    }

    return true;
}

