/*
* BackgroundTheme is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
* Copyright (c) 2017 Peter Morley.
*
* ReBoot is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, version 3.
*
* ReBoot is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/**
*  BackgroundTheme class. Manages an audio streamed played in a loop asynchronously.
*/

#pragma once
#include "fmod.hpp"

// (These functions do not block)
class BackgroundTheme {
public:
    BackgroundTheme() = default;
    ~BackgroundTheme();

    static FMOD_RESULT create(FMOD::System* pSystem,
                              const char* pFilename,
                              BackgroundTheme* pBg);

    // Starts the audio in the background - i.e. another thread
    FMOD_RESULT        playInBackground(FMOD::System* pSystem);
    FMOD_RESULT        pause();
    FMOD_RESULT        resume();

private:
    FMOD::Sound*       _sound   = nullptr;
    FMOD::Channel*     _channel = nullptr;
};
