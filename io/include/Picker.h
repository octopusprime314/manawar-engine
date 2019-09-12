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
    void                                     _mouseClick(int    button,
                                                         int    action,
                                                         int    x,
                                                         int    y);
    void                                     _mouseMove( double x,
                                                         double y);
    void                                     _editData(  int    x,
                                                         int    y,
                                                         bool   mouseDrag,
                                                         bool   mouseClick);
    void                                     _editTile(  int    x,
                                                         int    y,
                                                         int    entityID);


    MutableTextures                          _mutableTextureCache;
    std::function<bool(Entity*)>             _mouseDeleteCallback;
    bool                                     _leftMousePressed;
    int                                      _textureSelection;
    Vector4                                  _pixelEditValue;
    Vector4                                  _pickedPosition;
    int                                      _pickingRadius;
    std::function<bool(Vector4,bool)>        _mouseCallback;
    float*                                   _idBufferCache;
    Vector4                                  _mousePosition;
    std::vector<Entity*>                     _entityList;
    MRTFrameBuffer*                          _mrt;

public:
    Picker(MRTFrameBuffer*                   mrt,
           std::function<bool(Vector4,bool)> terminalCallback,
           std::function<bool(Entity*)>      mouseDeleteCallback);
    ~Picker();
    
    void                                     editTile(     int button,
                                                           int action,
                                                           int x,
                                                           int y,
                                                           int entityID);
    void                                     keyboardPress(int key,
                                                           int x,
                                                           int y);
    Vector4                                  getLastPickedPosition();
    void                                     saveMutableTextures();
    void                                     updateIdBuffer();
};
