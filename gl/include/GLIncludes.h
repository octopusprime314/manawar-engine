/*
* GLIncludes is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  GLIncludes. Manages gl based include files
*/

#pragma once

#include "GL/gl3w.h"
#include "GL/glfw3.h"
#include <string>
#include <cassert>


enum class GeometryConstruction {
    LINE_WIREFRAME,
    TRIANGLE_WIREFRAME,
    TRIANGLE_MESH
};


const std::string ASSET_LOCATION = "../assets/";
const std::string ANIMATED_MESH_LOCATION = ASSET_LOCATION + "meshes/animated/";
const std::string STATIC_MESH_LOCATION = ASSET_LOCATION + "meshes/static/";
const std::string SCENE_MESH_LOCATION = ASSET_LOCATION + "meshes/scene/";
const std::string TEXTURE_LOCATION = ASSET_LOCATION + "textures/";

// Set to true to make glCheck() assert when glGetError() returns an error code.
// This can be toggled in a debugger.
extern volatile bool g_AssertOnBadOpenGlCall;

#if defined(_DEBUG)
// Validate OpenGL calls. Call before and after any OpenGL API usage that
// you are worried about.
#define glCheck() do {                                                         \
    assert(glGetError != nullptr && "Don't call this before loading OpenGL");  \
    GLenum glError = GL_NO_ERROR;                                              \
    bool anyErrors = false;                                                    \
    while ((glError = glGetError()) != GL_NO_ERROR) {                          \
        anyErrors = true;                                                      \
        printf("[OpenGL] Error code 0x%X in %s(): %s:%d\n",                    \
            (int)glError,                                                      \
            __func__,                                                          \
            __FILE__,                                                          \
            __LINE__);                                                         \
    }                                                                          \
    if (g_AssertOnBadOpenGlCall) {                                             \
        assert(!anyErrors && "One or more OpenGL calls generated an error");   \
    }                                                                          \
} while (0)
#else
#define glCheck()
#endif