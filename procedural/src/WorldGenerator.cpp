#include "WorldGenerator.h"
#include "Terminal.h"
#include <iostream>

bool Builder::_allPathsFinished    = false;
int  Builder::_concurrentPaths     = 0;
int  Builder::_prevConcurrentPaths = 0;

// Reserve item flags in the bottom part of the integer and the rest represent unique path ids
int Builder::_pathIdAllocator = ItemFlag::TileFlagLength;

Builder::Builder(std::string sceneName,
                 int         pathId,
                 int         parentPathId,
                 int         widthLocation,
                 int         lengthLocation,
                 int         pathDirection,
                 int         tileWidthIndex,
                 int         tileLengthIndex)
    : _pathWidthLocation(widthLocation),
      _pathLengthLocation(lengthLocation),
      _tileWidthIndex(tileWidthIndex),
      _tileLengthIndex(tileLengthIndex),
      _proceduralGenDone(false),
      _currRotationInDegrees(pathDirection),
      _prevTileWidthIndex(tileWidthIndex),
      _prevTileLengthIndex(tileLengthIndex),
      _prevPathWidthLocation(widthLocation),
      _prevPathLengthLocation(lengthLocation),
      _sceneName(sceneName),
      _pathId(pathId),
      _parentPathId(parentPathId),
      _pathTileCount(0) {

    // Randomize initial path direction
    if (_currRotationInDegrees == -100000000) {
        _currRotationInDegrees = (rand() % rotationPathVariations) * rotationPathOffsetInDegrees;

        // Branch paths from all 3 other sides
        _pathGenerators.push_back(new Builder(_sceneName,
                                              _parentPathId,
                                              _pathId,
                                              _pathWidthLocation,
                                              _pathLengthLocation,
                                              _currRotationInDegrees + 180.0f,
                                              _tileWidthIndex,
                                              _tileLengthIndex));

        _pathGenerators.push_back(new Builder(_sceneName,
                                              _parentPathId,
                                              _pathId,
                                              _pathWidthLocation,
                                              _pathLengthLocation,
                                              _currRotationInDegrees + 90.0f,
                                              _tileWidthIndex,
                                              _tileLengthIndex));

        _pathGenerators.push_back(new Builder(_sceneName,
                                              _parentPathId,
                                              _pathId,
                                              _pathWidthLocation,
                                              _pathLengthLocation,
                                              _currRotationInDegrees + 270.0f,
                                              _tileWidthIndex,
                                              _tileLengthIndex));
    }
}

void Builder::_paintTile(int         widthLocation,
                         int         lengthLocation,
                         int         tileWidthIndex,
                         int         tileLengthIndex,
                         std::string name) {

    Terminal* terminal = Terminal::instance();

    int itemIndexW = ((tileWidthIndex * tileWidth) + widthLocation) / pathPixelRadius;
    int itemIndexL = ((tileLengthIndex * tileLength) + lengthLocation) / pathPixelRadius;

    // Unfortunately the resolution of items is pathPixelRadius so we need to upcast as well for now
    // Tag path id here.
    WorldGenerator::setItemId(itemIndexW,     itemIndexL,     _pathId);
    WorldGenerator::setItemId(itemIndexW + 1, itemIndexL,     _pathId);
    WorldGenerator::setItemId(itemIndexW - 1, itemIndexL,     _pathId);
    WorldGenerator::setItemId(itemIndexW,     itemIndexL + 1, _pathId);
    WorldGenerator::setItemId(itemIndexW,     itemIndexL - 1, _pathId);

    WorldGenerator::setItemId(itemIndexW - 1, itemIndexL - 1, _pathId);
    WorldGenerator::setItemId(itemIndexW - 1, itemIndexL + 1, _pathId);
    WorldGenerator::setItemId(itemIndexW + 1, itemIndexL - 1, _pathId);
    WorldGenerator::setItemId(itemIndexW + 1, itemIndexL + 1, _pathId);

    int id = WorldGenerator::getEntityId(tileWidthIndex, tileLengthIndex);

    std::string command = "MOUSEPATHING 0 1 ";
    command += std::to_string(widthLocation) + " ";
    command += std::to_string(lengthLocation) + " ";
    command += std::to_string(id) + " ";
    command += name;
    terminal->processCommand(command);
}

void Builder::_paintTiles(int         widthLocation,
                          int         lengthLocation,
                          int         tileWidthIndex,
                          int         tileLengthIndex,
                          std::string name) {

    // Paint current tile
    _paintTile(widthLocation, lengthLocation, tileWidthIndex, tileLengthIndex, name);

    // Checks to see if neighboring tiles need to be colored as well

    // Straddling tile to the right of current
    if (widthLocation >= (tileWidth - pathPixelRadius) && tileWidthIndex + 1 < numWidthTiles) {

        _paintTile(widthLocation - tileWidth, lengthLocation, tileWidthIndex + 1, tileLengthIndex, name);
    }
    // Straddling tile to the left of current
    if (widthLocation <= pathPixelRadius && tileWidthIndex - 1 >= 0) {

        _paintTile(tileWidth + widthLocation, lengthLocation, tileWidthIndex - 1, tileLengthIndex, name);
    }
    // Straddling tile to the top of current
    if (lengthLocation >= (tileLength - pathPixelRadius) && tileLengthIndex + 1 < numLengthTiles) {

        _paintTile(widthLocation, lengthLocation - tileLength, tileWidthIndex, tileLengthIndex + 1, name);
    }
    // Straddling tile to the bottom of current
    if (lengthLocation <= pathPixelRadius && tileLengthIndex - 1 >= 0) {

        _paintTile(widthLocation, tileLength + lengthLocation, tileWidthIndex, tileLengthIndex - 1, name);
    }
}

Builder::~Builder() {}

void Builder::buildPath() {

    // Build forked paths
    for (auto pathGen : _pathGenerators) {
        pathGen->buildPath();
    }

    if (_proceduralGenDone) {
        return;
    }

    // Keeps track of the number of live paths
    _concurrentPaths++;
    _allPathsFinished = false;

    // Utilize the terminal's ability to create and edit fbx files...
    // I know this should be lifted to a generic wrapper that edits the
    // fbx but for now I will inject string commands to the terminal interface.
    Terminal* terminal = Terminal::instance();

    // Second add paths and terrain painting of the tile
    // and make sure the path is within the extents of the tile terrain texture
    if ((_tileWidthIndex >= 0) &&
        (_tileWidthIndex < numWidthTiles) &&
        (_tileLengthIndex >= 0) &&
        (_tileLengthIndex < numLengthTiles)) {

        QuandrantBuilder quadrant =
            WorldGenerator::getQuadrant(_tileWidthIndex, _pathWidthLocation, _tileLengthIndex, _pathLengthLocation);

        // Tag path id for handling item placing.
        int prevTileWidthIndex  = ((_prevTileWidthIndex * tileWidth) + _prevPathWidthLocation) / pathPixelRadius;
        int prevTileLengthIndex = ((_prevTileLengthIndex * tileLength) + _prevPathLengthLocation) / pathPixelRadius;
        int tileWidthIndex      = ((_tileWidthIndex * tileWidth) + _pathWidthLocation) / pathPixelRadius;
        int tileLengthIndex     = ((_tileLengthIndex * tileLength) + _pathLengthLocation) / pathPixelRadius;
        int item                = WorldGenerator::getItemId(tileWidthIndex, tileLengthIndex);

        if ((item != 0) && (item != _pathId) && (item != _parentPathId)) {
            // Path intersects another path so terminate for the time being
            _proceduralGenDone = true;
        } else {

            if (highlightHeadOfPath) {

                // DEBUG PATH CODE
                // Paint previous path texture with the third texture option to indicate already traveled
                // Send a middle mouse button click to change texture
                terminal->processCommand("MOUSEPATHING 2 1 0 0 0");

                _paintTiles(_prevPathWidthLocation,
                            _prevPathLengthLocation,
                            _prevTileWidthIndex,
                            _prevTileLengthIndex,
                            _sceneName);

                // Reset texture back to path texture
                terminal->processCommand("MOUSEPATHING 2 1 0 0 0");
                terminal->processCommand("MOUSEPATHING 2 1 0 0 0");
                terminal->processCommand("MOUSEPATHING 2 1 0 0 0");
                // FINISHED DEBUG PATH CODE
            }
        }

        // Randomly generate a fork in the current path so pass in the current path location
        // Only if the minimum path size has been reached.
        if ((rand() % quadrant.probabilityToForkPath == 0) && (_pathTileCount >= minPathGenToCreatePath)) {

            // Change direction of forked path to be perpendicular to current path direction.
            _pathGenerators.push_back(new Builder(_sceneName,
                                                  Builder::getNewPathId(),
                                                  _pathId,
                                                  _pathWidthLocation,
                                                  _pathLengthLocation,
                                                  _currRotationInDegrees + ((rand() % 2 == 0) ? 90.0f : -90.0f),
                                                  _tileWidthIndex,
                                                  _tileLengthIndex));
        }

        _pathTileCount++;

        // Draw the current tile
        _paintTiles(_pathWidthLocation, _pathLengthLocation, _tileWidthIndex, _tileLengthIndex, _sceneName);

        _prevPathWidthLocation  = _pathWidthLocation;
        _prevPathLengthLocation = _pathLengthLocation;
        _prevTileWidthIndex     = _tileWidthIndex;
        _prevTileLengthIndex    = _tileLengthIndex;

        float       tileWidthMin  = (_tileWidthIndex * tileWidth) + minWidthValue - tileHalfWidth;
        float       tileLengthMin = (_tileLengthIndex * tileLength) + minLengthValue - tileHalfLength;
        std::string command       = "ADD " + _sceneName + " ";

        // Chance of placing a tree near the path
        bool placingHouse = ((rand() % quadrant.probabilityToPlaceHouse) == 0) ? true : false;

        // Make sure that a house does not terminate the procedural generation.
        // Always ensure that if placing a house, there is another path generator alive.
        // Also do not place houses on the seed paths because it will stunt procedural generation.
        if ((placingHouse == true) && (_prevConcurrentPaths > 1) && (_parentPathId != TileFlagLength)) {

            // Scale house model between 5 and 10
            float scale = static_cast<float>(rand() % 5) + 5.0f;

            // Rotate house around y axis
            float yRot = static_cast<float>(rand() % 360);

            command += "HOUSE2 ";
            command += std::to_string(tileWidthMin + _pathWidthLocation) + " ";
            command += std::to_string(0) + " "; // Keep height 0 for now
            command += std::to_string(tileLengthMin + _pathLengthLocation) + " ";
            command += std::to_string(scale) + " "; // w component for scaling
            command += std::to_string(0) + " ";
            command += std::to_string(yRot) + " "; // Rotation around y axis
            command += std::to_string(0) + " ";
            terminal->processCommand(command);

            // Flag grid location as having an item here
            WorldGenerator::setItemId(((_tileWidthIndex * tileWidth) + _pathWidthLocation) / pathPixelRadius,
                                      ((_tileLengthIndex * tileLength) + _pathLengthLocation) / pathPixelRadius,
                                      House);

            // Path leads to a house so terminate path
            _proceduralGenDone = true;
        }

        command = "ADD " + _sceneName + " ";
        // Chance of placing a tree near the path
        bool placingTree = ((rand() % quadrant.probabilityToPlaceTree) == 0) ? true : false;
        if (placingTree == true) {

            // Add trees around the path
            // The selection of trees we have are tree3, tree7 and tree8 for the time being
            int treeType = rand() % typesOfTrees;

            // Select tree type
            if (treeType == 0) {
                command += "TREE3 ";
            } else if (treeType == 1) {
                command += "TREE7 ";
            } else if (treeType == 2) {
                command += "TREE8 ";
            }

            // Scale model between 0.25 and 0.75
            float scale = ((static_cast<float>(rand()) / maxRandomValue) * 0.5f) + 0.25f;

            // Place to the left or right of the path, etc.
            // Add 90 degree rotation to make offset perpendicular to the direction of the path
            // Figure which perpendicular direction to place tree
            const float perpendicularDirection = (((rand() % 2) == 0) ? 1.0f : -1.0f);
            // Vary tree path offset with atleast 2 times the radius plus an extra offset between 0 and 2 in float
            const float radiusOffsetFromPath    = (static_cast<float>(rand() % 17) / 8.0f) + 2.0f;
            const int   treeLocationOffsetWidth = perpendicularDirection * (radiusOffsetFromPath * pathPixelRadius) *
                                                sin((_currRotationInDegrees + 90.0f) * degToRad);
            const int treeLocationOffsetLength = perpendicularDirection * (radiusOffsetFromPath * pathPixelRadius) *
                                                 cos((_currRotationInDegrees + 90.0f) * degToRad);

            command += std::to_string(tileWidthMin + _pathWidthLocation + treeLocationOffsetWidth) + " ";
            command += std::to_string(0) + " "; // Keep height 0 for now
            command += std::to_string(tileLengthMin + _pathLengthLocation + treeLocationOffsetLength) + " ";
            command += std::to_string(scale) + " "; // w component for scaling
            terminal->processCommand(command);

            // Flag grid location as having an item here
            WorldGenerator::setItemId(
                ((_tileWidthIndex * tileWidth) + _pathWidthLocation + treeLocationOffsetWidth) / pathPixelRadius,
                ((_tileLengthIndex * tileLength) + _pathLengthLocation + treeLocationOffsetLength) / pathPixelRadius,
                Tree);
        }

        // Find the next path location
        float proposedPathWidthLocation  = _pathWidthLocation;
        float proposedPathLengthLocation = _pathLengthLocation;

        // 0 degrees is North, 90 is East, 180 is South, 270 is West
        _currRotationInDegrees += ((rand() % 3) - 1) * rotationPathOffsetInDegrees;
        proposedPathLengthLocation += pathPixelRadius * cos(_currRotationInDegrees * degToRad);
        proposedPathWidthLocation += pathPixelRadius * sin(_currRotationInDegrees * degToRad);

        _pathWidthLocation  = proposedPathWidthLocation;
        _pathLengthLocation = proposedPathLengthLocation;

        if (_pathWidthLocation <= 0) {
            _tileWidthIndex--;
            // Location when jumping into another tile
            _pathWidthLocation += tileWidth;
        } else if (_pathWidthLocation >= tileWidth) {
            _tileWidthIndex++;
            // Location when jumping into another tile
            _pathWidthLocation = _pathWidthLocation % tileWidth;
        }
        if (_pathLengthLocation <= 0) {
            _tileLengthIndex--;
            // Location when jumping into another tile
            _pathLengthLocation += tileLength;
        } else if (_pathLengthLocation >= tileLength) {
            _tileLengthIndex++;
            // Location when jumping into another tile
            _pathLengthLocation = _pathLengthLocation % tileLength;
        }
    } else {
        _proceduralGenDone = true;
    }
}

int  WorldGenerator::_entityIDMap[numWidthTiles][numLengthTiles];
int  WorldGenerator::_tiledPathIds[numWidthPathIds][numLengthPathIds];
bool WorldGenerator::_dirtiedTiles[numWidthPathIds][numLengthPathIds];

QuandrantBuilder WorldGenerator::_builderQuadrants[numWidthQuadrants][numLengthQuadrants];
bool             WorldGenerator::_fileSaved           = false;
Builder*         WorldGenerator::_seedPath            = nullptr;
bool             WorldGenerator::_finishedClutterPass = false;

WorldGenerator::WorldGenerator(std::string sceneName) : _sceneName(sceneName) {}

WorldGenerator::~WorldGenerator() {}

void WorldGenerator::generateWorldTiles() {

    // Utilize the terminal's ability to create and edit fbx files...
    // I know this should be lifted to a generic wrapper that edits the
    // fbx but for now I will inject string commands to the terminal interface.

    Terminal* terminal = Terminal::instance();

    srand(time(NULL));

    const std::vector<Entity*>* entityList = nullptr;

    // Randomize quadrants prior to generating paths
    for (int quadrantIndexW = 0; quadrantIndexW < numWidthQuadrants; quadrantIndexW++) {
        for (int quadrantIndexL = 0; quadrantIndexL < numLengthQuadrants; quadrantIndexL++) {

            QuadrantFlag quadrant = static_cast<QuadrantFlag>(rand() % QuadrantFlag::QuadrantLength);

            if (quadrantIndexW == 0 && quadrantIndexL == 0) {
                _builderQuadrants[quadrantIndexW][quadrantIndexL] = quadrantBuilder[ForestQuadrant];
            }
            else if (quadrantIndexW == 1 && quadrantIndexL == 0) {
                _builderQuadrants[quadrantIndexW][quadrantIndexL] = quadrantBuilder[VillageQuadrant];
            }
            else if (quadrantIndexW == 0 && quadrantIndexL == 1) {
                _builderQuadrants[quadrantIndexW][quadrantIndexL] = quadrantBuilder[CityQuadrant];
            }
            else {
                _builderQuadrants[quadrantIndexW][quadrantIndexL] = quadrantBuilder[ForestQuadrant];
            }
        }
    }

    for (int tileIndexW = 0; tileIndexW < numWidthTiles; tileIndexW++) {
        for (int tileIndexL = 0; tileIndexL < numLengthTiles; tileIndexL++) {

            // First add a tile
            std::string command = "ADDTILE " + _sceneName + " TERRAINTILE ";
            command += std::to_string((tileIndexW * tileWidth) + minWidthValue) + " ";
            command += std::to_string(0) + " "; // Keep height 0 for now
            command += std::to_string((tileIndexL * tileLength) + minLengthValue) + " ";
            command += std::to_string(1) + " "; // w component for scaling

            // Determine which quadrant and give default texture
            int  quadrantIndexW = (tileIndexW * tileWidth)  / widthOfBuilderQuadrant;
            int  quadrantIndexL = (tileIndexL * tileLength) / lengthOfBuilderQuadrant;
            auto builder        = _builderQuadrants[quadrantIndexW][quadrantIndexL];

            if (builder.quadrantType == ForestQuadrant) {
                command += "SNOW.JPG DIRT.JPG ROCKS.JPG GRASS.JPG ";
            } else if (builder.quadrantType == CityQuadrant) {
                command += "SNOW.JPG BRICK.JPG DIRT.JPG ROCKS.JPG ";
            } else if (builder.quadrantType == VillageQuadrant) {
                command += "DIRT.JPG GRASS.JPG ROCKS.JPG DIRT.JPG ";
            }
            terminal->processCommand(command);

            // Get latest entity list based on the previous addition of a tile which is guaranteed to be at the end :)
            entityList = EngineManager::getEntityList();
            // Stores an entityID for every tile
            unsigned int entityID                = entityList->back()->getID();
            _entityIDMap[tileIndexW][tileIndexL] = entityID;
        }
    }

    // Small temporary hack that sets the texture for pathing and painting terrain to non background texture
    // Send a middle mouse button click to change texture
    terminal->processCommand("MOUSEPATHING 2 1 0 0 0");
    terminal->processCommand("MOUSEPATHING 2 1 0 0 0");

    // Increase the radius of the texture painting
    for (int i = 0; i < pathPixelRadius; i++) {
        terminal->processCommand("INCREASE_PAINT_SIZE");
    }
}

void WorldGenerator::clutterPass() {

    // Utilize the terminal's ability to create and edit fbx files...
    // I know this should be lifted to a generic wrapper that edits the
    // fbx but for now I will inject string commands to the terminal interface.

    Terminal* terminal = Terminal::instance();

    const std::vector<Entity*>* entityList = EngineManager::getEntityList();

    for (int tileIndexW = 0; tileIndexW < numWidthTiles; tileIndexW++) {
        for (int tileIndexL = 0; tileIndexL < numLengthTiles; tileIndexL++) {

            for (int tileLocationW = 0; tileLocationW < tileWidth; tileLocationW += pathPixelRadius) {
                for (int tileLocationL = 0; tileLocationL < tileLength; tileLocationL += pathPixelRadius) {

                    int locationW = (tileLocationW /*+ (rand() % pathPixelRadius)*/) % tileWidth;
                    int locationL = (tileLocationL /*+ (rand() % pathPixelRadius)*/) % tileLength;

                    QuandrantBuilder quadrant =
                        WorldGenerator::getQuadrant(tileIndexW, locationW, tileIndexL, locationL);

                    // Random tree placement
                    std::string command     = "ADD " + _sceneName + " ";
                    bool        placingTree = ((rand() % (quadrant.probabilityToPlaceTree)) == 0) ? true : false;
                    int         itemIndexW  = ((tileIndexW * tileWidth) + locationW) / pathPixelRadius;
                    int         itemIndexL  = ((tileIndexL * tileLength) + locationL) / pathPixelRadius;
                    int         itemId      = WorldGenerator::getItemId(itemIndexW, itemIndexL);

                    if ((placingTree == true) && (itemId == NoFlag)) {

                        // The selection of trees
                        int treeType = rand() % typesOfTrees;

                        // Select tree type
                        if (treeType == 0) {
                            command += "TREE3 ";
                        } else if (treeType == 1) {
                            command += "TREE7 ";
                        } else if (treeType == 2) {
                            command += "TREE8 ";
                        }

                        // Scale model between 0.25 and 0.75
                        float scale = ((static_cast<float>(rand()) / maxRandomValue) * 0.5f) + 0.25f;

                        command +=
                            std::to_string(((tileIndexW * tileWidth) + locationW) + minWidthValue - tileHalfWidth) +
                            " ";
                        command += std::to_string(0) + " "; // Keep height 0 for now
                        command +=
                            std::to_string(((tileIndexL * tileLength) + locationL) + minLengthValue - tileHalfLength) +
                            " ";
                        command += std::to_string(scale) + " "; // w component for scaling
                        terminal->processCommand(command);

                        // Flag grid location as having an item here
                        WorldGenerator::setItemId(itemIndexW, itemIndexL, Tree);
                    }
                }
            }
        }
    }
    _finishedClutterPass = true;
}

void WorldGenerator::spawnPaths(std::string sceneName) {

    // seed path
    static WorldGenerator* worldSeed = nullptr;

    if (worldSeed == nullptr) {
        worldSeed = new WorldGenerator(sceneName);
        worldSeed->generateWorldTiles();

        _seedPath = new Builder(sceneName, Builder::getNewPathId());
    }

    if (Builder::isAllPathsFinished() == true) {

        if (_finishedClutterPass == false) {
            worldSeed->clutterPass();
        }

        if (_fileSaved == false) {
            Terminal* terminal = Terminal::instance();
            terminal->processCommand("SAVE " + sceneName);
            _fileSaved = true;
        }
    } else {

        if (_fileSaved == false) {
            Builder::initPathingData();
            _seedPath->buildPath();
            Builder::updatePathCount();
        }
    }
}