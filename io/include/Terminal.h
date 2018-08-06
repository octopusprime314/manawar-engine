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
#include "UpdateInterface.h"
#include "MRTFrameBuffer.h"

class Picker;

class Terminal {
public:

    Terminal(MRTFrameBuffer* gBuffers, std::vector<Entity*> entityList);
    ~Terminal();

    void                 display();

private:
    FontRenderer         _fontRenderer;
    std::string          _commandString;
    std::string          _commandToProcess;
    std::vector<std::string> _commandHistory;
    int                      _commandHistoryIndex;
    void                 _updateKeyboard(int key, int x, int y);
    void                 _updateGameState(int state);

    int                  _getCursorIndex();
    static ShaderBroker* _shaderManager; 
    static ModelBroker*  _modelManager;
    int                  _gameState;
    Picker*              _picker;


    void                 _mousePosition(Vector4 position);
};