#include "Water.h"
#include "MasterClock.h"
#include <random>
#include "TextureBroker.h"

Water::Water(ViewEvents* eventWrapper) :
    Effect(eventWrapper, "waterShader", EffectType::Water) {

    const std::string noiseTextureName = "../assets/textures/water/noise.png";
    auto texBroker = TextureBroker::instance();
    texBroker->addTexture(noiseTextureName);
    _noiseTexture = texBroker->getTexture(noiseTextureName);

    MasterClock::instance()->subscribeKinematicsRate(std::bind(&Water::_updateTime, this, std::placeholders::_1));
}

void Water::render() {
    //Bring the time back to real time for the effects shader
    //The amount of milliseconds in 24 hours
    const uint64_t dayLengthMilliseconds = 24 * 60 * 60 * 1000;
    uint64_t updateTimeAmplified = dayLengthMilliseconds / (60 * 1000);
    float realTimeMilliSeconds = static_cast<float>(_milliSecondTime) / static_cast<float>(updateTimeAmplified);
    _effectShader->runShader(this, realTimeMilliSeconds / 1000.f);
}

GLuint Water::getNoiseTexture() {
    return _noiseTexture->getContext();
}
