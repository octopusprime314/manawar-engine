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

enum TileDirection {
    None                = 0,
    Left                = 1,
    Right               = 2,
    Up                  = 3,
    Down                = 4,
    TileDirectionLength = 5
};

struct TileData {
    TileID tileId;
};

void generateScene(std::string sceneName);