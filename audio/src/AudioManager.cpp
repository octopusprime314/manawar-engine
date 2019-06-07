#include "AudioManager.h"

#define THEME_MP3 "assets/audio/forest.mp3"

#include <Windows.h>
#include <iostream>

AudioManager::AudioManager() {
    // Core System
    FMOD_RESULT result;
    _system = nullptr;
    result = FMOD::System_Create(&_system);
    if (result != FMOD_OK) { __debugbreak(); }

    result = _system->init(1, FMOD_INIT_NORMAL, /*extra*/ nullptr);
    if (result != FMOD_OK) { __debugbreak(); }

    // Background theme
    result = BackgroundTheme::create(_system, THEME_MP3, &_backgroundTheme);
    if (result == FMOD_ERR_FILE_NOTFOUND) {
        // Maybe we're in a build directory, and need to go up a level?
        result = BackgroundTheme::create(_system, "../" THEME_MP3, &_backgroundTheme);
    }
    if (result != FMOD_OK) {
        if (result == FMOD_ERR_FILE_NOTFOUND) {
            std::cout << "Unable to find \"" THEME_MP3 "\" or \"" "../" THEME_MP3 "\"\n";
        } else if (result != FMOD_OK) {
            std::cout << "Error Creating Background Theme";
        }
    }
}

AudioManager::~AudioManager() {
    _system->release();
    _system = nullptr;
}

FMOD::System* AudioManager::getAudioSystem() { 
    return _system; 
}

BackgroundTheme& AudioManager::getBackgroundTheme() { 
    return _backgroundTheme; 
}

FMOD_RESULT AudioManager::startAll() {
    return _backgroundTheme.playInBackground(_system);
}
