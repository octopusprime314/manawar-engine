/*
* Terminal is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  Terminal class. Used to hold the font renderer to display io a gui terminal
*/

#pragma once
#include "Font.h"
#include "ShaderBroker.h"
#include "ModelBroker.h"
#include "EventSubscriber.h"
#include "MRTFrameBuffer.h"

class Picker;

class Terminal {
public:

    static Terminal* instance();
    void             initTerminal(  MRTFrameBuffer*      gBuffers,
                                    std::vector<Entity*> entityList);
    void             processCommand(std::string command);
    void             processCommands();

  private:
    Terminal();
    ~Terminal();

    bool                     _mousePosition(Vector4 position, bool mouseClick);
    void                     _updateReleaseKeyboard(int key, int x, int y);
    void                     _updateGameState(EngineStateFlags state);
    void                     _updateKeyboard(int key, int x, int y);
    void                     _commandProcessor(std::string command);
    bool                     _mouseDeletion(Entity* entity);
    int                      _getCursorIndex();

    size_t                   _commandHistoryIndex;
    static Terminal*         _terminalInstance;
    std::string              _commandToProcess;
    bool                     _shiftKeyPressed;
    int                      _modelNameIndex;
    std::vector<std::string> _commandHistory;
    std::string              _commandString;
    static ShaderBroker*     _shaderManager; 
    static ModelBroker*      _modelManager;
    FontRenderer             _fontRenderer;
    EngineStateFlags         _gameState;
    MRTFrameBuffer*          _gBuffers;
    Picker*                  _picker;
};