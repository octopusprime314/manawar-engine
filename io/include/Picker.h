/*
* Picker is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  Picker uses a g buffer to identify which entity asset is being clicked
*/

#pragma once
#include "Entity.h"
#include "MRTFrameBuffer.h"
#include "MutableTexture.h"
#include <functional>
#include <unordered_map>
using MutableTextures = std::unordered_map<std::string, MutableTexture*>;

class Picker {
    MRTFrameBuffer*                   _mrt;
    void                              _mouseClick(int button, int action, int x, int y);
    void                              _mouseMove(double x, double y);
    void                              _editData(int x, int y, bool mouseDrag, bool mouseClick);
    void                              _keyboardPress(int key, int x, int y);

    std::vector<Entity*>              _entityList;
    int                               _textureSelection;
    Vector4                           _pixelEditValue;
    int                               _pickingRadius;
    std::function<bool(Vector4,bool)> _mouseCallback;
    bool                              _leftMousePressed;
    MutableTextures                   _mutableTextureCache;
    float*                            _idBufferCache;
    Vector4                           _mousePosition;
    Vector4                           _pickedPosition;
public:
    Picker(MRTFrameBuffer* mrt, std::function<bool(Vector4,bool)> terminalCallback);
    void updateIdBuffer();
    void saveMutableTextures();
    Vector4 getLastPickedPosition();
    ~Picker();
};
