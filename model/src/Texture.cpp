#include "Texture.h"

Texture::Texture(){

}

Texture::Texture(std::string textureName){

    //image format
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    //pointer to the image, once loaded
    FIBITMAP *dib(0);
    //pointer to the image data
    BYTE* bits(0);

    //check the file signature and deduce its format
    fif = FreeImage_GetFileType(textureName.c_str(), 0);

    //if still unknown, try to guess the file format from the file extension
    if(fif == FIF_UNKNOWN) {
        fif = FreeImage_GetFIFFromFilename(textureName.c_str());
    }
    //if still unkown, return failure
    if(fif == FIF_UNKNOWN){
        return;
    }

    //check that the plugin has reading capabilities and load the file
    if(FreeImage_FIFSupportsReading(fif)) {
        dib = FreeImage_Load(fif, textureName.c_str());
    }
    //if the image failed to load, return failure
    if(!dib) {
        return;
    }

    //retrieve the image data
    bits = FreeImage_GetBits(dib);
    //get the image width and height
    int width = FreeImage_GetWidth(dib);
    int height = FreeImage_GetHeight(dib);
    //if this somehow one of these failed (they shouldn't), return failure
    if((bits == 0) || (width == 0) || (height == 0)) {
        return;
    }

    //Generate a texture context
    glGenTextures(1, &_textureContext);

    //Bind the generated reference context to load texture data
    glBindTexture(GL_TEXTURE_2D, _textureContext);

    unsigned int imageSize = FreeImage_GetMemorySize(dib);
    unsigned int byteSize = width*height*4;

    bool alphaValues = true;
    if(byteSize > imageSize) {
        alphaValues = false;
    }

    //Use mip maps to prevent anti aliasing issues
    if(textureName.substr(textureName.find_last_of('.')) == ".tif" && alphaValues) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, bits);
        glGenerateMipmap(GL_TEXTURE_2D); //Allocate mipmaps 
    }
    else if(textureName.substr(textureName.find_last_of('.')) == ".tif" && !alphaValues) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, bits);
        glGenerateMipmap(GL_TEXTURE_2D); //Allocate mipmaps
    }
    else if(textureName.substr(textureName.find_last_of('.')) == ".dds" && alphaValues) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, bits);
        glGenerateMipmap(GL_TEXTURE_2D); //Allocate mipmaps
    }
    else if(textureName.substr(textureName.find_last_of('.')) == ".dds" && !alphaValues) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0,  GL_BGR, GL_UNSIGNED_BYTE, bits);
        glGenerateMipmap(GL_TEXTURE_2D); //Allocate mipmaps
    }
    else if(textureName.substr(textureName.find_last_of('.')) == ".tga" && alphaValues) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, bits);
        glGenerateMipmap(GL_TEXTURE_2D); //Allocate mipmaps
    }
    else if(textureName.substr(textureName.find_last_of('.')) == ".tga" && !alphaValues) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, bits);
        glGenerateMipmap(GL_TEXTURE_2D); //Allocate mipmaps
    }
    else if(textureName.substr(textureName.find_last_of('.')) == ".jpg" && alphaValues) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, bits);
        glGenerateMipmap(GL_TEXTURE_2D); //Allocate mipmaps
    }
    else if(textureName.substr(textureName.find_last_of('.')) == ".jpg" && !alphaValues) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0,  GL_BGR, GL_UNSIGNED_BYTE, bits);
        glGenerateMipmap(GL_TEXTURE_2D); //Allocate mipmaps
    }
    else if(textureName.substr(textureName.find_last_of('.')) == ".bmp" && alphaValues) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, bits);
        glGenerateMipmap(GL_TEXTURE_2D); //Allocate mipmaps
    }
    else if(textureName.substr(textureName.find_last_of('.')) == ".bmp" && !alphaValues) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, bits);
        glGenerateMipmap(GL_TEXTURE_2D); //Allocate mipmaps
    }
    else if(textureName.substr(textureName.find_last_of('.')) == ".png" && alphaValues) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bits);
        glGenerateMipmap(GL_TEXTURE_2D); //Allocate mipmaps
    }
    else if(textureName.substr(textureName.find_last_of('.')) == ".png" && !alphaValues) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, bits);
        glGenerateMipmap(GL_TEXTURE_2D); //Allocate mipmaps
    }

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);

    //Enable anisotropic filtering which prevent bluring of low res mip map textures
    //Preserves high texture resolution regardless
    float aniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &aniso);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, aniso); 

    //Free FreeImage's copy of the data
    FreeImage_Unload(dib);
}

Texture::~Texture(){

}

GLuint Texture::getContext(){
    return _textureContext;
}

