/*
* Effect is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  Effect class. Base class for effects that occur on a 2D plane including water and fire
*/

#pragma once
#include "MVP.h"
#include "EventSubscriber.h"
#include "Shader.h"
#include "EffectShader.h"

enum class EffectType {
    None  = 0,
    Water = 1,
    Fire  = 2,
    Smoke = 3
};

class Effect : public EventSubscriber {

public:
    Effect(ViewEvents* eventWrapper,
        std::string shaderName,
        EffectType effectType);

    virtual void  render() = 0;

    MVP           getCameraMVP();
    EffectType    getType();

protected:
    void          _updateReleaseKeyboard(int key, int x, int y) {};
    void          _updateGameState(EngineStateFlags state) {};
    void          _updateKeyboard(int key, int x, int y) {};
    void          _updateProjection(Matrix projection);
    void          _updateMouse(double x, double y) {};
    void          _updateView(Matrix view);
    void          _updateTime(int time);
    void          _updateDraw() {};

    uint64_t      _milliSecondTime;
    EffectShader* _effectShader;
    EffectType    _effectType;
    MVP           _cameraMVP;
};
