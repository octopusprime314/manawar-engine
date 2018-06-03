#include "Effect.h"
#include "MasterClock.h"
#include <random>

Effect::Effect(ViewManagerEvents* eventWrapper, std::string shaderName, EffectType effectType) :
    UpdateInterface(eventWrapper),
    _effectShader(shaderName) ,
    _effectType(effectType) {
}

MVP Effect::getCameraMVP() {
    return _cameraMVP;
}
EffectType Effect::getType() {
    return _effectType;
}

void Effect::_updateDraw() {
}
void Effect::_updateKeyboard(int key, int x, int y) {
}
void Effect::_updateReleaseKeyboard(int key, int x, int y) {
}
void Effect::_updateMouse(double x, double y) {
}

void Effect::_updateView(Matrix view) {
    _cameraMVP.setView(view);
}

void Effect::_updateProjection(Matrix projection) {
    _cameraMVP.setProjection(projection);
}

void Effect::_updateTime(int time) {

    //The amount of milliseconds in 24 hours
    const uint64_t dayLengthMilliseconds = 24 * 60 * 60 * 1000;

    //Every update comes in real time so in order to speed up
    //we need to multiply that value by some constant
    //A full day takes one minute should do it lol
    //divide total time by 60 seconds times 1000 to convert to milliseconds
    uint64_t updateTimeAmplified = dayLengthMilliseconds / (60 * 1000);

    _milliSecondTime += (updateTimeAmplified*time);
    _milliSecondTime %= dayLengthMilliseconds;
}
