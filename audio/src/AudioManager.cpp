#include "AudioManager.h"

#define THEME_MP3 "assets/audio/BeachTheme.mp3"

#include <Windows.h>
#include <iostream>

AudioManager::AudioManager()
{
    // Core System
    FMOD_RESULT result;
    m_pSystem = nullptr;
    result = FMOD::System_Create(&m_pSystem);
    if (result != FMOD_OK) { __debugbreak(); }

    result = m_pSystem->init(1, FMOD_INIT_NORMAL, /*extra*/ nullptr);
    if (result != FMOD_OK) { __debugbreak(); }

    // Background theme
    result = BackgroundTheme::Create(m_pSystem, THEME_MP3, &m_backgroundTheme);
    if (result == FMOD_ERR_FILE_NOTFOUND) {
        // Maybe we're in a build directory, and need to go up a level?
        result = BackgroundTheme::Create(m_pSystem, "../" THEME_MP3, &m_backgroundTheme);
    }
    if (result != FMOD_OK) {
        if (result == FMOD_ERR_FILE_NOTFOUND) {
            std::cout << "Unable to find \"" THEME_MP3 "\" or \"" "../" THEME_MP3 "\"\n";
        } else if (result != FMOD_OK) {
            std::cout << "Error Creating Background Theme";
        }
    }
}

AudioManager::~AudioManager()
{
    m_pSystem->release();
    m_pSystem = nullptr;
}

FMOD_RESULT AudioManager::StartAll()
{
    return m_backgroundTheme.PlayInBackground(m_pSystem);
}
