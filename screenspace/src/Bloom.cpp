#include "Bloom.h"
#include "SSCompute.h"
#include "GLIncludes.h"

Bloom::Bloom() {

    _luminanceFilter = new SSCompute("highLuminanceFilter", Format::RGBUB);
    _horizontalBlur = new SSCompute("blurHorizontalShaderRGB", Format::RGBUB);
    _verticalBlur = new SSCompute("blurVerticalShaderRGB", Format::RGBUB);
}

Bloom::~Bloom() {

}

unsigned int Bloom::getTextureContext() {
    //Grab the last iteration of the blur
    return _verticalBlur->getTextureContext();
}

void Bloom::compute(unsigned int deferredFBOTexture) {

    //Luminance threshold test
    _luminanceFilter->compute(deferredFBOTexture);

    //Do a horizontal and then a vertical blur pass!
    _horizontalBlur->compute(_luminanceFilter->getTextureContext());

    //Blur 4 more times!
    for (int i = 0; i < 4; i++) {
        _verticalBlur->compute(_horizontalBlur->getTextureContext());
        _horizontalBlur->compute(_verticalBlur->getTextureContext());
    }
    _verticalBlur->compute(_horizontalBlur->getTextureContext());
}