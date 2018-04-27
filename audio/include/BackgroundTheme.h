#pragma once

#include "fmod.hpp"

// Manages an audio streamed played in a loop asynchronously.
// (These functions do not block)
class BackgroundTheme {
public:
    static FMOD_RESULT Create(FMOD::System* pSystem,
                             const char* pFilename,
                             BackgroundTheme* pBg);
    BackgroundTheme() = default;
    ~BackgroundTheme();

    // Starts the audio in the background - i.e. another thread
    FMOD_RESULT PlayInBackground(FMOD::System* pSystem);
    FMOD_RESULT Pause();
    FMOD_RESULT Resume();

private:
    FMOD::Sound*   pSound   = nullptr;
    FMOD::Channel* pChannel = nullptr;
};
