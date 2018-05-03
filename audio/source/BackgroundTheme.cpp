#include "BackgroundTheme.h"

FMOD_RESULT BackgroundTheme::Create(FMOD::System* pSystem,
                                    const char* pFilename,
                                    BackgroundTheme* pBg) {
    return pSystem->createStream(pFilename,
                                 FMOD_LOOP_NORMAL | FMOD_3D,
                                 nullptr,
                                 &pBg->pSound);
}

BackgroundTheme::~BackgroundTheme() {
    // lol i dunno
}

FMOD_RESULT BackgroundTheme::PlayInBackground(FMOD::System* pSystem) {
    FMOD_RESULT result;
    result = pSystem->playSound(pSound,
                       /*ch group*/ nullptr,
                       /*paused?*/ false,
                       &pChannel);
    if (result == FMOD_OK) {
        result = pChannel->setVolume(1.0f);
    }
    return result;
}

FMOD_RESULT BackgroundTheme::Pause() {
    return pChannel->setPaused(true);
}

FMOD_RESULT BackgroundTheme::Resume() {
    return pChannel->setPaused(false);
}
