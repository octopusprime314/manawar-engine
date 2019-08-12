/*
* EngineState is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  EngineState class.  Manages the state of the engine in terms of editing the world or running the game
*  with editing disabled.  New editing features will need to be added here.
*/

#pragma once

struct EngineStateFlags {
    bool worldEditorModeEnabled;
    bool geometryOctTreeEnabled;
    bool geometryVisualEnabled;
    bool frustumVisualEnabled;
    bool renderOctTreeEnabled;
    bool gameModeEnabled;
};

class EngineState {

public:
    ~EngineState();
    static void                    setEngineState(EngineStateFlags engineState);
    static const EngineStateFlags& getEngineState();

private:
    EngineState();
    static EngineStateFlags        _engineStateFlags;
};
