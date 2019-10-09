#include "MutableTexture.h"
#include "TextureBroker.h"

MutableTexture::MutableTexture() {}

MutableTexture::MutableTexture(std::string textureName, int width, int height)
    : Texture(TEXTURE_LOCATION + textureName + ".tif", width, height), _bitmapToWrite(nullptr), _originalData(nullptr) {
    _createTextureData();
}

MutableTexture::MutableTexture(std::string originalTexture, std::string clonedTexture)
    : Texture(originalTexture,
              TextureBroker::instance()->getAssetTextureFromLayered(originalTexture)->getWidth(),
              TextureBroker::instance()->getAssetTextureFromLayered(originalTexture)->getHeight()),
      _bitmapToWrite(nullptr), _originalData(nullptr) {
    _cloneTexture(TEXTURE_LOCATION + clonedTexture + ".tif");
}

MutableTexture::MutableTexture(std::string textureName)
    : Texture(textureName,
              TextureBroker::instance()->getAssetTextureFromLayered(textureName)->getWidth(),
              TextureBroker::instance()->getAssetTextureFromLayered(textureName)->getHeight()),
      _bitmapToWrite(nullptr), _originalData(nullptr) {}

MutableTexture::~MutableTexture() {}

void MutableTexture::_cloneTexture(std::string newName) {

    // Only makes TIFF format for now
    bool bSuccess = false;
    auto bitmap   = FreeImage_Allocate(_width, _height, 32, 8, 8, 8);

    // image format bmp for now
    FREE_IMAGE_FORMAT fif = FIF_TIFF;

    int textureWidth  = FreeImage_GetWidth(bitmap);
    int textureHeight = FreeImage_GetHeight(bitmap);

    for (int y = textureHeight - 1; y >= 0; y--) {
        BYTE* bits = FreeImage_GetScanLine(bitmap, textureHeight - 1 - y);
        for (int x = 0; x < textureWidth; x++) {
            bits[0] = 0; // blue
            bits[1] = 0; // green
            bits[2] = 0; // red
            bits[3] = 0; // alpha
            bits += 4;   // pointer math
        }
    }

    bool success = FreeImage_Save(fif, bitmap, newName.c_str(), TIFF_DEFAULT);
    FreeImage_Unload(bitmap);

    if (!success) {
        std::cout << "Texture creation failed: " << _name << std::endl;
    }
}

void MutableTexture::_createTextureData() {

    // Only makes TIFF format for now
    bool bSuccess = false;
    auto bitmap   = FreeImage_Allocate(_width, _height, 32, 8, 8, 8);

    // image format bmp for now
    FREE_IMAGE_FORMAT fif = FIF_TIFF;

    int textureWidth  = FreeImage_GetWidth(bitmap);
    int textureHeight = FreeImage_GetHeight(bitmap);

    for (int y = textureHeight - 1; y >= 0; y--) {
        BYTE* bits = FreeImage_GetScanLine(bitmap, textureHeight - 1 - y);
        for (int x = 0; x < textureWidth; x++) {
            bits[0] = 0;   // blue
            bits[1] = 0;   // green
            bits[2] = 0;   // red
            bits[3] = 255; // alpha
            bits += 4;     // pointer math
        }
    }

    bool success = FreeImage_Save(fif, bitmap, _name.c_str(), TIFF_DEFAULT);
    FreeImage_Unload(bitmap);

    if (!success) {
        std::cout << "Texture creation failed: " << _name << std::endl;
    }
}

void MutableTexture::saveToDisk() {
    if (_bitmapToWrite != nullptr) {
        // image format tiff
        FREE_IMAGE_FORMAT fif     = FIF_TIFF;
        bool              success = FreeImage_Save(fif, _bitmapToWrite, (_name).c_str(), TIFF_DEFAULT);
        // FreeImage_Unload(_bitmapToWrite);
        if (!success) {
            std::cout << "Texture creation failed: " << _name << std::endl;
        }
    }
}

void MutableTexture::editTextureData(int xPosition, int yPosition, Vector4 texturePixel, bool tempChange, int radius) {

    auto texture       = TextureBroker::instance()->getAssetTextureFromLayered(_name);
    int  textureWidth  = texture->getWidth();
    int  textureHeight = texture->getHeight();

    if (xPosition >= -radius && xPosition < (textureWidth + radius) &&
        yPosition >= -radius && yPosition < (textureHeight + radius)) {

        FIBITMAP* bitmap = nullptr;
        if (_originalData == nullptr) {
            auto size     = texture->getSizeInBytes();
            _originalData = new BYTE[size];
            memcpy(_originalData, texture->getBits(), size);
        }
        if (_bitmapToWrite == nullptr) {
            _bitmapToWrite = FreeImage_Allocate(textureWidth, textureHeight, 32, 8, 8, 8);
        }

        BYTE* bitmapToRead = nullptr;
        if (tempChange) {
            bitmapToRead = _originalData;
        } else {
            bitmapToRead = texture->getBits();
        }

        // image format bmp for now
        FREE_IMAGE_FORMAT fif = FIF_TIFF;

        for (int y = textureHeight - 1; y >= 0; y--) {
            BYTE* bits = FreeImage_GetScanLine(_bitmapToWrite, textureHeight - 1 - y);
            for (int x = 0; x < textureWidth; x++) {

                int    deltaX         = abs(x - xPosition);
                int    deltaY         = abs(y - yPosition);

                // Use for circular painting on a texture
                double computedRadius = sqrt(static_cast<double>(deltaX * deltaX + deltaY * deltaY));
                if (computedRadius <= radius) {
                //if (deltaX <= radius &&
                //    deltaY <= radius) {
                    bits[0] = static_cast<unsigned int>(texturePixel.getx()); // blue
                    bits[1] = static_cast<unsigned int>(texturePixel.gety()); // green
                    bits[2] = static_cast<unsigned int>(texturePixel.getz()); // red
                    bits[3] = static_cast<unsigned int>(texturePixel.getw()); // alpha
                } else {
                    bits[0] = bitmapToRead[0];
                    bits[1] = bitmapToRead[1];
                    bits[2] = bitmapToRead[2];
                    bits[3] = bitmapToRead[3];
                }
                bits         += 4; // pointer math
                bitmapToRead += 4;
            }
        }
        if (_originalData != nullptr && tempChange == false) {
            auto size = texture->getSizeInBytes();
            memcpy(_originalData, FreeImage_GetBits(_bitmapToWrite), size);
        }
        TextureBroker::instance()->updateTextureToLayered(_name, _bitmapToWrite);
    }
}