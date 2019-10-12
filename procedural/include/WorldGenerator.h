#pragma once
#include "EngineManager.h"
#include <string>

enum TileFlag {
    NoFlag         = 0,
    Path           = 1,
    Item           = 2,
    TileFlagLength = 3
};

// First define the extents of the world and various other world building constants
const int   tileWidth              = 200;
const int   tileLength             = 200;
const int   tileHalfWidth          = tileWidth / 2;
const int   tileHalfLength         = tileLength / 2;
const int   widthOfWorld           = 10000;
const int   lengthOfWorld          = 10000;
const int   heightOfWorld          = 10000;
const int   numWidthTiles          = widthOfWorld / tileWidth;
const int   numLengthTiles         = lengthOfWorld / tileLength;
const int   halfWidthTiles         = numWidthTiles / 2;
const int   halfLengthTiles        = numLengthTiles / 2;
const int   typesOfTrees           = 3;
const int   numTreesToPlace        = 1;
const float maxRandomValue         = static_cast<float>(RAND_MAX);
const int   pathPixelRadius        = 10;
const int   pathPixelDiameter      = pathPixelRadius * 2;
const int   probabilityToPlaceItem = 20; // 1/20
const int   probabilityToForkPath  = 100; // 1/100

// Radial pathing data
// 360 degrees of rotation for the next path direction
const int   fullCircleInDegrees         = 360;
const int   rotationPathOffsetInDegrees = 5;
const int   rotationPathVariations      = fullCircleInDegrees / rotationPathOffsetInDegrees;
const float degToRad                    = PI / 180.0f;

class WorldGenerator {

public:

    static void spawnPaths(std::string sceneName);

    WorldGenerator(std::string sceneName,
                   int         widthLocation   = tileHalfWidth,
                   int         lengthLocation  = tileHalfLength,
                   int         pathDirection   = 0,
                   int         tileWidthIndex  = numWidthTiles  / 2,
                   int         tileLengthIndex = numLengthTiles / 2);
    ~WorldGenerator();

    void generateWorldTiles();

private:

    void paintTile(int          widthLocation,
                   int          lengthLocation,
                   int          id,
                   std::string  name);

    void paintTiles(int         widthLocation,
                    int         lengthLocation,
                    int         tileWidthIndex,
                    int         tileLengthIndex,
                    std::string name);

    // Debug aid to observe how the path creation is done
    void buildPath();

    int                          _prevPathLengthLocation;
    int                          _prevPathWidthLocation;
    int                          _currRotationInDegrees;
    int                          _prevTileLengthIndex;
    int                          _prevTileWidthIndex;
    int                          _pathLengthLocation;
    int                          _pathWidthLocation;
    bool                         _proceduralGenDone;
    int                          _tileLengthIndex;
    int                          _tileWidthIndex;
    std::vector<WorldGenerator*> _pathGenerators;
    int                          _direction[2];
    std::string                  _sceneName;

    static bool                  _allPathsFinished;
    static bool                  _fileSaved;
    // Flags used to identify what type of object is at this location in the grid
    static TileFlag _tileGridFlag[widthOfWorld / pathPixelRadius][lengthOfWorld / pathPixelRadius];

    // Identifies a tile within the grid's entity ID for access
    static int      _entityIDMap[numWidthTiles][numLengthTiles];
};