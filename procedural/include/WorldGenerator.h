#pragma once
#include "EngineManager.h"
#include <string>

enum ItemFlag {
    NoFlag         = 0,
    Path           = 1,
    Tree           = 2,
    House          = 3,
    TileFlagLength = 4
};

// First define the extents of the world and various other world building constants
constexpr int   tileWidth               = 200;
constexpr int   tileLength              = 200;
constexpr int   tileHalfWidth           = tileWidth  / 2;
constexpr int   tileHalfLength          = tileLength / 2;
constexpr int   widthOfWorld            = 5000;
constexpr int   lengthOfWorld           = 5000;
constexpr int   heightOfWorld           = 5000;
constexpr int   numWidthTiles           = widthOfWorld   / tileWidth;
constexpr int   numLengthTiles          = lengthOfWorld  / tileLength;
constexpr int   halfWidthTiles          = numWidthTiles  / 2;
constexpr int   halfLengthTiles         = numLengthTiles / 2;
constexpr int   typesOfTrees            = 3;
constexpr int   numTreesToPlace         = 1;
constexpr float maxRandomValue          = static_cast<float>(RAND_MAX);
constexpr int   pathPixelRadius         = 10;
constexpr int   pathPixelDiameter       = pathPixelRadius * 2;
constexpr int   probabilityToPlaceTree  = 20;
constexpr int   probabilityToPlaceHouse = 200;
constexpr int   probabilityToForkPath   = 100;

// Radial pathing data
// 360 degrees of rotation for the next path direction
constexpr int   fullCircleInDegrees         = 360;
constexpr int   rotationPathOffsetInDegrees = 5;
constexpr int   rotationPathVariations      = fullCircleInDegrees / rotationPathOffsetInDegrees;
constexpr float degToRad                    = PI / 180.0f;

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
                   int          tileWidthIndex,
                   int          tileLengthIndex,
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
    std::string                  _sceneName;

    static int                   _prevConcurrentPaths;
    static bool                  _allPathsFinished;
    static int                   _concurrentPaths;
    static bool                  _fileSaved;

    // Identifies a tile within the grid's entity ID for access
    static int      _entityIDMap[numWidthTiles][numLengthTiles];
    // Flags used to identify what type of object is at this location in the grid
    static ItemFlag _tileGridFlag[widthOfWorld / pathPixelRadius][lengthOfWorld / pathPixelRadius];
};