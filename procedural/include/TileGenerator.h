#pragma once
#include <string>

// Currently not using enum class because of strongly typing

// Indicates what type of geometry will be included in the tile
// No coincidence with magic the gathering mana :)
enum TileID {
    Plains         = 0,
    Swamp          = 1,
    Forest         = 2,
    Mountain       = 3,
    Island         = 4,
    TileIdLength   = 5
};

enum TileFlag { 
    NoFlag         = 0,
    Path           = 1,
    Item           = 2,
    TileFlagLength = 3
};

enum TileDirection {
    North               = 0,
    East                = 1,
    South               = 2,
    West                = 3,
    NorthEast           = 4,
    SouthEast           = 5,
    NorthWest           = 6,
    SouthWest           = 7,
    TileDirectionLength = 8
};

struct TileData {
    TileID tileId;
};

void generateScene(std::string sceneName);
// Debug aid to observe how the path creation is done
void updatePath(   std::string sceneName);