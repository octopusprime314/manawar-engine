#include "MutableTexture.h"
#include "TextureBroker.h"

MutableTexture::MutableTexture() {

}

MutableTexture::MutableTexture(std::string textureName, int width, int height) :
    Texture(TEXTURE_LOCATION + textureName + ".tif", width, height) {
    _createTextureData();
}

MutableTexture::MutableTexture(std::string textureName) : 
    Texture(textureName, 
            TextureBroker::instance()->getAssetTextureFromLayered(textureName)->getWidth(),
            TextureBroker::instance()->getAssetTextureFromLayered(textureName)->getHeight()) {

}

MutableTexture::~MutableTexture() {

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

void MutableTexture::editTextureData(int xPosition, int yPosition, Vector4 texturePixel) {

    auto texture = TextureBroker::instance()->getAssetTextureFromLayered(_name);
    int textureWidth = texture->getWidth();
    int textureHeight = texture->getHeight();

    if (xPosition >= 0 && xPosition < textureWidth && yPosition >= 0 && yPosition < textureHeight) {
        auto bitmapToRead = texture->getBits();

        auto bitmapToWrite = FreeImage_Allocate(textureWidth, textureHeight, 32, 8, 8, 8);
        //image format bmp for now
        FREE_IMAGE_FORMAT fif = FIF_TIFF;

        for (int y = textureHeight - 1; y >= 0; y--) {
            BYTE *bits = FreeImage_GetScanLine(bitmapToWrite, textureHeight - 1 - y);
            for (int x = 0; x < textureWidth; x++) {

                if (x == xPosition && y == yPosition) {
                    bits[0] = static_cast<unsigned int>(texturePixel.getz()); //blue
                    bits[1] = static_cast<unsigned int>(texturePixel.gety()); //green
                    bits[2] = static_cast<unsigned int>(texturePixel.getx()); //red
                    bits[3] = static_cast<unsigned int>(texturePixel.getw()); //alpha
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

        bool success = FreeImage_Save(fif, bitmapToWrite, (texture->getName()).c_str(), TIFF_DEFAULT);
        FreeImage_Unload(bitmapToWrite);

        TextureBroker::instance()->updateTextureToLayered(_name);

        if (!success) {
            std::cout << "Texture creation failed: " << _name << std::endl;
        }
    }
}