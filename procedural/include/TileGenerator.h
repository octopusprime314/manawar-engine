#pragma once
#include <string>

// Indicates what type of geometry will be included in the tile
// No coincidence with magic the gathering mana :)
enum class TileID {
    PLAINS   = 0,
    SWAMP    = 1,
    FOREST   = 2,
    MOUNTAIN = 3,
    ISLAND   = 4,
    LENGTH   = 5
};

struct TileData {
    TileID tileId;
};

void generateScene(std::string sceneName);