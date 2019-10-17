#include "WorldGenerator.h"
#include "Terminal.h"
#include <iostream>



int      WorldGenerator::_entityIDMap[numWidthTiles][numLengthTiles];
int      WorldGenerator::_tiledPathIds[widthOfWorld / pathPixelRadius][lengthOfWorld / pathPixelRadius];

bool     WorldGenerator::_allPathsFinished    = false;
bool     WorldGenerator::_fileSaved           = false;
int      WorldGenerator::_concurrentPaths     = 0;
int      WorldGenerator::_prevConcurrentPaths = 0;

// Reserve item flags in the bottom part of the integer and the rest represent unique path ids
int      WorldGenerator::_pathIdAllocator     = ItemFlag::TileFlagLength;


WorldGenerator::WorldGenerator(std::string sceneName,
                               int         widthLocation,
                               int         lengthLocation,
                               int         pathDirection,
                               int         tileWidthIndex,
                               int         tileLengthIndex)
    :
    _pathWidthLocation     (widthLocation),
    _pathLengthLocation    (lengthLocation),
    _tileWidthIndex        (tileWidthIndex),
    _tileLengthIndex       (tileLengthIndex),
    _proceduralGenDone     (false),
    _currRotationInDegrees (pathDirection),
    _prevTileWidthIndex    (tileWidthIndex),
    _prevTileLengthIndex   (tileLengthIndex),
    _prevPathWidthLocation (widthLocation),
    _prevPathLengthLocation(lengthLocation),
    _sceneName             (sceneName),
    _pathId                (_pathIdAllocator) {

    // Increment path id
    _pathIdAllocator++;
}

void WorldGenerator::paintTile(int         widthLocation,
                               int         lengthLocation,
                               int         tileWidthIndex,
                               int         tileLengthIndex,
                               std::string name) {

    Terminal* terminal = Terminal::instance();

    int id = _entityIDMap[tileWidthIndex][tileLengthIndex];

    std::string command = "MOUSEPATHING 0 1 ";
    command += std::to_string(widthLocation)  + " ";
    command += std::to_string(lengthLocation) + " ";
    command += std::to_string(id)             + " ";
    command += name;
    terminal->processCommand(command);
}

void WorldGenerator::paintTiles(int         widthLocation,
                                int         lengthLocation,
                                int         tileWidthIndex,
                                int         tileLengthIndex,
                                std::string name) {

    // Paint current tile
    paintTile(widthLocation,
              lengthLocation,
              tileWidthIndex,
              tileLengthIndex,
              name);

    //Checks to see if neighboring tiles need to be colored as well

    // Straddling tile to the right of current
    if (widthLocation      >= (tileWidth - pathPixelRadius) &&
        tileWidthIndex + 1 < numWidthTiles) {

        paintTile(widthLocation - tileWidth,
                  lengthLocation,
                  tileWidthIndex + 1,
                  tileLengthIndex,
                  name);
    }
    // Straddling tile to the left of current
    if (widthLocation      <= pathPixelRadius &&
        tileWidthIndex - 1 >= 0) {

        paintTile(tileWidth + widthLocation,
                  lengthLocation,
                  tileWidthIndex - 1,
                  tileLengthIndex,
                  name);
    }

    // Straddling tile to the top of current
    if (lengthLocation      >= (tileLength - pathPixelRadius) &&
        tileLengthIndex + 1 < numLengthTiles) {

        paintTile(widthLocation,
                  lengthLocation - tileLength,
                  tileWidthIndex,
                  tileLengthIndex + 1,
                  name);
    }

    // Straddling tile to the bottom of current
    if (lengthLocation      <= pathPixelRadius &&
        tileLengthIndex - 1 >= 0) {

        paintTile(widthLocation,
                  tileLength + lengthLocation,
                  tileWidthIndex,
                  tileLengthIndex - 1,
                  name);
    }
}

WorldGenerator::~WorldGenerator() {

}

void WorldGenerator::generateWorldTiles() {

    // Utilize the terminal's ability to create and edit fbx files...
    // I know this should be lifted to a generic wrapper that edits the 
    // fbx but for now I will inject string commands to the terminal interface.

    Terminal* terminal = Terminal::instance();

    srand(time(NULL));

    // Randomize initial path direction
    _currRotationInDegrees = (rand() % rotationPathVariations) * rotationPathOffsetInDegrees;

    const std::vector<Entity*>* entityList = nullptr;

    for (int tileIndexW = -halfWidthTiles; tileIndexW <= halfWidthTiles; tileIndexW++) {
        for (int tileIndexL = -halfLengthTiles; tileIndexL <= halfLengthTiles; tileIndexL++) {

            // First add a tile
            std::string command = "ADDTILE " + _sceneName + " FOREST ";
            command            += std::to_string(tileIndexW * tileWidth)  + " ";
            command            += std::to_string(0)                       + " "; // Keep height 0 for now
            command            += std::to_string(tileIndexL * tileLength) + " ";
            command            += std::to_string(1)                       + " "; // w component for scaling
            command            += "SNOW.JPG DIRT.JPG ROCKS.JPG GRASS.JPG";
            terminal->processCommand(command);

            // Get latest entity list based on the previous addition of a tile which is guaranteed to be at the end :)
            entityList   = EngineManager::getEntityList();
            // Stores an entityID for every tile
            unsigned int entityID = entityList->back()->getID();
            _entityIDMap[tileIndexW + halfWidthTiles][tileIndexL + halfLengthTiles] = entityID;
        }
    }

    // Small temporary hack that sets the texture for pathing and painting terrain to non background texture
    // Send a middle mouse button click to change texture
    terminal->processCommand("MOUSEPATHING 2 1 0 0 0");

    // Increase the radius of the texture painting
    for (int i = 0; i < pathPixelRadius; i++) {
        terminal->processCommand("INCREASE_PAINT_SIZE");
    }
}

void WorldGenerator::spawnPaths(std::string sceneName) {

    // seed path
    static WorldGenerator* seedPath = nullptr;

    if (seedPath == nullptr) {
        seedPath = new WorldGenerator(sceneName);
        seedPath->generateWorldTiles();
    }

    if ((_allPathsFinished == true) &&
        (_fileSaved        == false)) {
        Terminal* terminal = Terminal::instance();
        terminal->processCommand("SAVE " + sceneName);
        _fileSaved = true;
    }

    if (_fileSaved == false) {
        _concurrentPaths  = 0;
        _allPathsFinished = true;
        seedPath->buildPath();
        _prevConcurrentPaths = _concurrentPaths;
    }
}

void WorldGenerator::buildPath() {

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

    // Randomly generate a fork in the current path so pass in the current path location
    if (rand() % probabilityToForkPath == 0) {

        // Change direction of forked path to be perpendicular to current path direction.
        _pathGenerators.push_back(new WorldGenerator(_sceneName,
                                                     _pathWidthLocation,
                                                     _pathLengthLocation,
                                                     _currRotationInDegrees + ((rand() % 2 == 0) ? 90.0f : -90.0f),
                                                     _tileWidthIndex,
                                                     _tileLengthIndex));
    }

    // Utilize the terminal's ability to create and edit fbx files...
    // I know this should be lifted to a generic wrapper that edits the 
    // fbx but for now I will inject string commands to the terminal interface.
    Terminal* terminal  = Terminal::instance();

    // Second add paths and terrain painting of the tile
    // and make sure the path is within the extents of the tile terrain texture
    if ((_tileWidthIndex  >= 0)             &&
        (_tileWidthIndex  <  numWidthTiles) &&
        (_tileLengthIndex >= 0)             &&
        (_tileLengthIndex <  numLengthTiles)) {

        if ((_pathWidthLocation  >= 0)         &&
            (_pathWidthLocation  <= tileWidth) &&
            (_pathLengthLocation >= 0)         &&
            (_pathLengthLocation <= tileLength)) {


            // Find the next path location
            float proposedPathWidthLocation  = _pathWidthLocation;
            float proposedPathLengthLocation = _pathLengthLocation;

            // 0 degrees is North, 90 is East, 180 is South, 270 is West
            _currRotationInDegrees     += ((rand() % 3) - 1) * rotationPathOffsetInDegrees;
            proposedPathLengthLocation += pathPixelRadius * cos(_currRotationInDegrees * degToRad);
            proposedPathWidthLocation  += pathPixelRadius * sin(_currRotationInDegrees * degToRad);

            _pathWidthLocation  = proposedPathWidthLocation;
            _pathLengthLocation = proposedPathLengthLocation;

            if (_pathWidthLocation <= 0) {
                _tileWidthIndex--;
                // Location when jumping into another tile
                _pathWidthLocation += tileWidth;
            }
            else if (_pathWidthLocation >= tileWidth) {
                _tileWidthIndex++;
                // Location when jumping into another tile
                _pathWidthLocation = _pathWidthLocation % tileWidth;
            }
            if (_pathLengthLocation <= 0) {
                _tileLengthIndex--;
                // Location when jumping into another tile
                _pathLengthLocation += tileLength;
            }
            else if (_pathLengthLocation >= tileLength) {
                _tileLengthIndex++;
                // Location when jumping into another tile
                _pathLengthLocation = _pathLengthLocation % tileLength;
            }

            // Tag path id for handling item placing.
            int      prevTileWidthIndex  = ((_prevTileWidthIndex  * tileWidth)  + _prevPathWidthLocation)  / pathPixelRadius;
            int      prevTileLengthIndex = ((_prevTileLengthIndex * tileLength) + _prevPathLengthLocation) / pathPixelRadius;
            int      tileWidthIndex      = ((_tileWidthIndex      * tileWidth)  + _pathWidthLocation)      / pathPixelRadius;
            int      tileLengthIndex     = ((_tileLengthIndex     * tileLength) + _pathLengthLocation)     / pathPixelRadius;
            int      item                = _tiledPathIds[tileWidthIndex][tileLengthIndex];

            
            if ((item != 0) && (item != _pathId)) {
                // Path intersects another path so terminate for the time being
                _proceduralGenDone = true;
            }
            else {
                // DEBUG PATH CODE
                // Paint previous path texture with the third texture option to indicate already traveled
                // Send a middle mouse button click to change texture
                terminal->processCommand("MOUSEPATHING 2 1 0 0 0");

                paintTiles(_prevPathWidthLocation,
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

            // Draw the current tile
            paintTiles(_pathWidthLocation,
                       _pathLengthLocation,
                       _tileWidthIndex,
                       _tileLengthIndex,
                       _sceneName);

            
            // Only tag main tile for path id.
            _tiledPathIds[tileWidthIndex][tileLengthIndex] = _pathId;

            _prevPathWidthLocation  = _pathWidthLocation;
            _prevPathLengthLocation = _pathLengthLocation;
            _prevTileWidthIndex     = _tileWidthIndex;
            _prevTileLengthIndex    = _tileLengthIndex;

            float tileWidthMin  = ((_tileWidthIndex  - halfWidthTiles)  * tileWidth)  - tileHalfWidth;
            float tileLengthMin = ((_tileLengthIndex - halfLengthTiles) * tileLength) - tileHalfLength;
            std::string command = "ADD " + _sceneName + " ";

            // Chance of placing a tree near the path
            bool placingHouse = ((rand() % probabilityToPlaceHouse) == 0) ? true : false;

            // Make sure that a house does not terminate the procedural generation.
            // Always ensure that if placing a house, there is another path generator alive.
            if ((placingHouse         == true) &&
                (_prevConcurrentPaths >  1)) {

                // Scale house model between 5 and 10
                float scale = static_cast<float>(rand() % 5) + 5.0f;

                // Rotate house around y axis
                float yRot  = static_cast<float>(rand() % 360);

                command += "HOUSE2 ";
                command += std::to_string(tileWidthMin  + _pathWidthLocation)  + " ";
                command += std::to_string(0)                                   + " "; // Keep height 0 for now
                command += std::to_string(tileLengthMin + _pathLengthLocation) + " ";
                command += std::to_string(scale)                               + " "; // w component for scaling
                command += std::to_string(0)                                   + " ";
                command += std::to_string(yRot)                                + " "; // Rotation around y axis
                command += std::to_string(0)                                   + " ";
                terminal->processCommand(command);

                // Flag grid location as having an item here
                _tiledPathIds[((_tileWidthIndex  * tileWidth)  + _pathWidthLocation)  / pathPixelRadius]
                             [((_tileLengthIndex * tileLength) + _pathLengthLocation) / pathPixelRadius] = House;

                // Path leads to a house so terminate path
                _proceduralGenDone = true;
            }

            // Chance of placing a tree near the path
            bool placingTree = ((rand() % probabilityToPlaceTree) == 0) ? true : false;
            if (placingTree == true) {

                // Add trees around the path
                // The selection of trees we have are tree3, tree7 and tree8 for the time being
                int treeType = rand() % typesOfTrees;

                // Select tree type
                if (treeType == 0) {
                    command += "TREE3 ";
                }
                else if (treeType == 1) {
                    command += "TREE7 ";
                }
                else if (treeType == 2) {
                    command += "TREE8 ";
                }

                // Scale model between 0.25 and 0.75
                float scale = ((static_cast<float>(rand()) / maxRandomValue) * 0.5f) + 0.25f;

                // Place to the left or right of the path, etc.
                // Add 90 degree rotation to make offset perpendicular to the direction of the path
                // Figure which perpendicular direction to place tree 
                const float perpendicularDirection = (((rand() % 2) == 0) ? 1.0f : -1.0f);
                // Vary tree path offset with atleast 2 times the radius plus an extra offset between 0 and 2 in float
                const float radiusOffsetFromPath   = (static_cast<float>(rand() % 17) / 8.0f) + 2.0f;
                const int treeLocationOffsetWidth  = perpendicularDirection * (radiusOffsetFromPath * pathPixelRadius) * sin((_currRotationInDegrees + 90.0f) * degToRad);
                const int treeLocationOffsetLength = perpendicularDirection * (radiusOffsetFromPath * pathPixelRadius) * cos((_currRotationInDegrees + 90.0f) * degToRad);

                command += std::to_string(tileWidthMin  + _pathWidthLocation  + treeLocationOffsetWidth)  + " ";
                command += std::to_string(0)                                                              + " "; // Keep height 0 for now
                command += std::to_string(tileLengthMin + _pathLengthLocation + treeLocationOffsetLength) + " ";
                command += std::to_string(scale)                                                          + " "; // w component for scaling
                terminal->processCommand(command);

                // Flag grid location as having an item here
                _tiledPathIds[((_tileWidthIndex  * tileWidth)  + _pathWidthLocation  + treeLocationOffsetWidth)  / pathPixelRadius]
                             [((_tileLengthIndex * tileLength) + _pathLengthLocation + treeLocationOffsetLength) / pathPixelRadius] = Tree;

            }
        }
    }
    else {
        _proceduralGenDone = true;
    }
}
