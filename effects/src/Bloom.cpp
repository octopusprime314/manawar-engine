#include "Bloom.h"
#include "SSCompute.h"
#include "GLIncludes.h"
#include "IOEventDistributor.h"
#include "Texture.h"

Bloom::Bloom() {

    _luminanceFilter = new SSCompute("highLuminanceFilter",
                                     IOEventDistributor::screenPixelWidth,
                                     IOEventDistributor::screenPixelHeight,
                                     TextureFormat::RGBA_UNSIGNED_BYTE);

    _horizontalBlur  = new SSCompute("blurHorizontalShaderRGB",
                                     IOEventDistributor::screenPixelWidth  / 4,
                                     IOEventDistributor::screenPixelHeight / 4,
                                     TextureFormat::RGBA_UNSIGNED_BYTE);

    _verticalBlur    = new SSCompute("blurVerticalShaderRGB",
                                     IOEventDistributor::screenPixelWidth  / 4,
                                     IOEventDistributor::screenPixelHeight / 4,
                                     TextureFormat::RGBA_UNSIGNED_BYTE);
    
    //downsampling shader
    _downSample      = new SSCompute("downsampleRGB",
                                     IOEventDistributor::screenPixelWidth  / 4,
                                     IOEventDistributor::screenPixelHeight / 4,
                                     TextureFormat::RGBA_UNSIGNED_BYTE);

    //upsampling shader
    _upSample        = new SSCompute("upsampleRGB",
                                     IOEventDistributor::screenPixelWidth,
                                     IOEventDistributor::screenPixelHeight,
                                     TextureFormat::RGBA_UNSIGNED_BYTE);
}

Bloom::~Bloom() {

}

unsigned int Bloom::getTextureContext() {
    return _upSample->getTextureContext();
}

Texture* Bloom::getTexture() {
    return _upSample->getTexture();
}

void Bloom::compute(Texture* deferredFBOTexture) {

    //Luminance threshold test
    _luminanceFilter->compute(deferredFBOTexture);
    _luminanceFilter->uavBarrier();

    _downSample->compute(_luminanceFilter->getTexture());
    _downSample->uavBarrier();

    //Do a horizontal and then a vertical blur pass!
    _horizontalBlur->compute(_downSample->getTexture());
    _horizontalBlur->uavBarrier();

    //Blur 4 more times!
    for (int i = 0; i < 4; i++) {
        _verticalBlur->compute(_horizontalBlur->getTexture());
        _verticalBlur->uavBarrier();
        _horizontalBlur->compute(_verticalBlur->getTexture());
        _horizontalBlur->uavBarrier();
    }
    _verticalBlur->compute(_horizontalBlur->getTexture());
    _verticalBlur->uavBarrier();

    _upSample->compute(_verticalBlur->getTexture());
    _upSample->uavBarrier();
}

