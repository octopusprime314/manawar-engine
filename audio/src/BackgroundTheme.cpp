#include "BackgroundTheme.h"

FMOD_RESULT BackgroundTheme::create(FMOD::System* pSystem,
                                    const char* pFilename,
                                    BackgroundTheme* pBg) {
    return pSystem->createStream(pFilename,
                                 FMOD_LOOP_NORMAL | FMOD_3D,
                                 nullptr,
                                 &pBg->_sound);
}

BackgroundTheme::~BackgroundTheme()
{
    // lol i dunno
}

FMOD_RESULT BackgroundTheme::playInBackground(FMOD::System* system) {
    FMOD_RESULT result;
    result = system->playSound(_sound,
                       /*ch group*/ nullptr,
                       /*paused?*/ false,
                       &_channel);
    if (result == FMOD_OK) {
        result = _channel->setVolume(1.0f);
    }
    return result;
}

FMOD_RESULT BackgroundTheme::pause() {
    return _channel->setPaused(true);
}

FMOD_RESULT BackgroundTheme::resume() {
    return _channel->setPaused(false);
}
