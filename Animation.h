/*
* Animation is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  Animation class. Stores AnimationFrames object that contains
*  the individual animation frame skin transforms within the entire
*  animation set.
*/

#pragma once
#include <string>
#include "AnimationFrames.h"
#include "FbxLoader.h"
class Animation {

    AnimationFrames _frames; //Contains the skin/bone transforms that compose an entire animation frame

public:
	Animation();
	void addSkin(SkinningData* skin);
};