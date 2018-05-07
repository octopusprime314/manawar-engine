#include "AudioManager.h"

#define THEME_MP3 "assets/audio/Theme.mp3"

#include <Windows.h>

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
        MessageBoxA(nullptr, "Unable to load " THEME_MP3 ".", "Unable to find assets", MB_OK);
    } else if (result != FMOD_OK) {
        MessageBoxA(nullptr, "Unable to create background theme.",
            "Error Creating Background Theme",
            MB_OK);
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