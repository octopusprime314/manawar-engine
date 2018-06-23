#pragma once

#include "BackgroundTheme.h"

#include "fmod.hpp"

class AudioManager
{
public:
    AudioManager();
    ~AudioManager();

    FMOD::System* GetAudioSystem() { return m_pSystem; }
    BackgroundTheme& GetBackgroundTheme() { return m_backgroundTheme; }

    FMOD_RESULT StartAll();

private:
    FMOD::System* m_pSystem;
    BackgroundTheme m_backgroundTheme;
};