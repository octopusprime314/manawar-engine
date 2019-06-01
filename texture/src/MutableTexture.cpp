#include "MutableTexture.h"
#include "TextureBroker.h"

MutableTexture::MutableTexture() {

}

MutableTexture::MutableTexture(std::string textureName, int width, int height) :
    Texture(TEXTURE_LOCATION + textureName + ".tif", width, height),
    _bitmapToWrite(nullptr),
    _originalData(nullptr) {
    _createTextureData();
}

MutableTexture::MutableTexture(std::string originalTexture, std::string clonedTexture) :
    Texture(originalTexture,
        TextureBroker::instance()->getAssetTextureFromLayered(originalTexture)->getWidth(),
        TextureBroker::instance()->getAssetTextureFromLayered(originalTexture)->getHeight()),
    _bitmapToWrite(nullptr),
    _originalData(nullptr) {
    _cloneTexture(TEXTURE_LOCATION + clonedTexture + ".tif");
}

MutableTexture::MutableTexture(std::string textureName) : 
    Texture(textureName, 
            TextureBroker::instance()->getAssetTextureFromLayered(textureName)->getWidth(),
            TextureBroker::instance()->getAssetTextureFromLayered(textureName)->getHeight()),
    _bitmapToWrite(nullptr),
    _originalData(nullptr) {

}

MutableTexture::~MutableTexture() {

}

void MutableTexture::_cloneTexture(std::string newName) {

    //Only makes TIFF format for now
    bool bSuccess = false;
    auto bitmap = FreeImage_Allocate(_width, _height, 32, 8, 8, 8);

    //image format bmp for now
    FREE_IMAGE_FORMAT fif = FIF_TIFF;

    int textureWidth = FreeImage_GetWidth(bitmap);
    int textureHeight = FreeImage_GetHeight(bitmap);

    for (int y = textureHeight - 1; y >= 0; y--) {
        BYTE *bits = FreeImage_GetScanLine(bitmap, textureHeight - 1 - y);
        for (int x = 0; x < textureWidth; x++) {
            bits[0] = 0; //blue
            bits[1] = 0; //green
            bits[2] = 0; //red
            bits[3] = 0; //alpha
            bits += 4; //pointer math
        }
    }

    bool success = FreeImage_Save(fif, bitmap, newName.c_str(), TIFF_DEFAULT);
    FreeImage_Unload(bitmap);

    if (!success) {
        std::cout << "Texture creation failed: " << _name << std::endl;
    }
}

void MutableTexture::_createTextureData() {
    
    //Only makes TIFF format for now
    bool bSuccess = false;
    auto bitmap = FreeImage_Allocate(_width, _height, 32, 8, 8, 8);
    
    //image format bmp for now
    FREE_IMAGE_FORMAT fif = FIF_TIFF;

    int textureWidth = FreeImage_GetWidth(bitmap);
    int textureHeight = FreeImage_GetHeight(bitmap);

    for (int y = textureHeight - 1; y >= 0; y--) {
        BYTE *bits = FreeImage_GetScanLine(bitmap, textureHeight - 1 - y);
        for (int x = 0; x < textureWidth; x++) {
            bits[0] = 0; //blue
            bits[1] = 0; //green
            bits[2] = 0; //red
            bits[3] = 255; //alpha
            bits += 4; //pointer math
        }
    }

    bool success = FreeImage_Save(fif, bitmap, _name.c_str(), TIFF_DEFAULT);
    FreeImage_Unload(bitmap);

    if (!success) {
        std::cout << "Texture creation failed: " << _name << std::endl;
    }
}

void MutableTexture::saveToDisk() {
    //image format tiff
    FREE_IMAGE_FORMAT fif = FIF_TIFF;
    bool success = FreeImage_Save(fif, _bitmapToWrite, (_name).c_str(), TIFF_DEFAULT);
    FreeImage_Unload(_bitmapToWrite);
    if (!success) {
        std::cout << "Texture creation failed: " << _name << std::endl;
    }
}

void MutableTexture::editTextureData(int xPosition, int yPosition, Vector4 texturePixel, bool tempChange, int radius) {

    auto texture = TextureBroker::instance()->getAssetTextureFromLayered(_name);
    int textureWidth = texture->getWidth();
    int textureHeight = texture->getHeight();

    if (xPosition >= 0 && xPosition < textureWidth && yPosition >= 0 && yPosition < textureHeight) {
        
        FIBITMAP* bitmap = nullptr;
        if (_originalData == nullptr) {
            auto size = texture->getSizeInBytes();
            _originalData = new BYTE[size];
            memcpy(_originalData, texture->getBits(), size);
        }
        if (_bitmapToWrite == nullptr) {
            _bitmapToWrite = FreeImage_Allocate(textureWidth, textureHeight, 32, 8, 8, 8);
        }

        BYTE* bitmapToRead = nullptr;
        if (tempChange) {
            bitmapToRead = _originalData;
        }
        else {
            bitmapToRead = texture->getBits();
        }

        //image format bmp for now
        FREE_IMAGE_FORMAT fif = FIF_TIFF;

        for (int y = textureHeight - 1; y >= 0; y--) {
            BYTE *bits = FreeImage_GetScanLine(_bitmapToWrite, textureHeight - 1 - y);
            for (int x = 0; x < textureWidth; x++) {
                if (x >= xPosition - radius && x <= xPosition + radius
                    && y >= yPosition - radius && y <= yPosition + radius) {

                    int deltaX = abs(x - xPosition);
                    int deltaY = abs(y - yPosition);
                    float max = (deltaX > deltaY ? deltaX : deltaY) + 1.0f;

                    Vector4 adjustedTexel = Vector4(texturePixel.getx() / max,
                        texturePixel.gety() / max,
                        texturePixel.getz() / max,
                        texturePixel.getw());

                    auto r = static_cast<unsigned int>(adjustedTexel.getx()) + static_cast<unsigned int>(bitmapToRead[2]);
                    auto g = static_cast<unsigned int>(adjustedTexel.gety()) + static_cast<unsigned int>(bitmapToRead[1]);
                    auto b = static_cast<unsigned int>(adjustedTexel.getz()) + static_cast<unsigned int>(bitmapToRead[0]);
                    auto a = adjustedTexel.getw();
                    r = r > 255 ? 255 : r;
                    g = g > 255 ? 255 : g;
                    b = b > 255 ? 255 : b;

                    r = adjustedTexel.getx() == 0 ? 0 : r;
                    g = adjustedTexel.gety() == 0 ? 0 : g;
                    b = adjustedTexel.getz() == 0 ? 0 : b;

                    bits[0] = static_cast<unsigned int>(b); //blue
                    bits[1] = static_cast<unsigned int>(g); //green
                    bits[2] = static_cast<unsigned int>(r); //red
                    bits[3] = static_cast<unsigned int>(a); //alpha
                }
                else {
                    bits[0] = bitmapToRead[0];
                    bits[1] = bitmapToRead[1];
                    bits[2] = bitmapToRead[2];
                    bits[3] = bitmapToRead[3];
                }
                bits += 4; //pointer math
                bitmapToRead += 4;
            }
        }
        if (_originalData != nullptr && tempChange == false) {
            auto size = texture->getSizeInBytes();
            memcpy(_originalData, FreeImage_GetBits(_bitmapToWrite), size);
        }
        TextureBroker::instance()->updateTextureToLayered(_name, static_cast<void*>(FreeImage_GetBits(_bitmapToWrite)));
    }
}