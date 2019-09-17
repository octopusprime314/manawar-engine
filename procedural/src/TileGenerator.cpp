#include "TileGenerator.h"
#include "Terminal.h"
#include "EngineManager.h"
#include <iostream>

// First define the extents of the world and use the hardcoded tile size of 200x200
const int   tileWidth       = 200;
const int   tileLength      = 200;
const int   tileHalfWidth   = tileWidth / 2;
const int   tileHalfLength  = tileLength / 2;
const int   widthOfWorld    = 1000;
const int   lengthOfWorld   = 1000;
const int   heightOfWorld   = 1000;
const int   numWidthTiles   = widthOfWorld / tileWidth;
const int   numLengthTiles  = lengthOfWorld / tileLength;
const int   halfWidthTiles  = numWidthTiles / 2;
const int   halfLengthTiles = numLengthTiles / 2;
const int   typesOfTrees    = 3;
const int   numTreesToPlace = 5;
const float maxRandomValue  = static_cast<float>(RAND_MAX);
const int   pathPixelRadius = 10;
const int   pathPixelDiameter      = pathPixelRadius * 2;
const int   probabilityToPlaceItem = 20; // 1/20

int  pathWidthLocation  = tileHalfWidth;
int  pathLengthLocation = tileHalfLength;
int  tileWidthIndex     = numWidthTiles  / 2;
int  tileLengthIndex    = numLengthTiles / 2;
int  direction[2]       = {0, 0};
bool proceduralGenDone  = false;

int prevTileWidthIndex     = tileWidthIndex;
int prevTileLengthIndex    = tileLengthIndex;
int prevPathWidthLocation  = pathWidthLocation;
int prevPathLengthLocation = pathLengthLocation;

// Identifies a tile within the grid's entity ID for access
int entityIDMap[numWidthTiles][numLengthTiles];

// Flags used to identify what type of object is at this location in the grid
TileFlag tileGridFlag[widthOfWorld / pathPixelDiameter][lengthOfWorld / pathPixelDiameter];

void paintTile(int widthLocation, int lengthLocation, int id, std::string name) {

    Terminal* terminal = Terminal::instance();

    std::string command = "MOUSEPATHING 0 1 ";
    command += std::to_string(widthLocation) + " ";
    command += std::to_string(lengthLocation) + " ";
    command += std::to_string(id) + " ";
    command += name;
    terminal->processCommand(command);
}

void paintTiles(int         widthLocation,
                int         lengthLocation,
                int         tileWidthIndex,
                int         tileLengthIndex,
                std::string name) {

    // Paint current tile
    paintTile(widthLocation,
              lengthLocation,
              entityIDMap[tileWidthIndex][tileLengthIndex],
              name);

    //Checks to see if neighboring tiles need to be colored as well

    // Strattling tile to the right of current
    if (widthLocation >= tileWidth) {

        paintTile(0,
                  lengthLocation,
                  entityIDMap[tileWidthIndex + 1][tileLengthIndex],
                  name);
    }
    // Strattling tile to the left of current
    if (widthLocation <= 0) {

        paintTile(tileWidth,
                  lengthLocation,
                  entityIDMap[tileWidthIndex - 1][tileLengthIndex],
                  name);
    }
    // Strattling tile to the top of current
    if (lengthLocation >= tileLength) {

        paintTile(widthLocation,
                  0,
                  entityIDMap[tileWidthIndex][tileLengthIndex + 1],
                  name);
    }
    // Strattling tile to the bottom of current
    if (lengthLocation <= 0) {

        paintTile(widthLocation,
                  tileLength,
                  entityIDMap[tileWidthIndex][tileLengthIndex - 1],
                  name);
    }
    
    // Check for corners
    if (widthLocation  >= tileWidth &&
        lengthLocation >= tileLength) {

        paintTile(0,
                  0,
                  entityIDMap[tileWidthIndex + 1][tileLengthIndex + 1],
                  name);
    }
    else if (widthLocation  >= tileWidth &&
             lengthLocation <= 0) {

        paintTile(0,
                  tileLength,
                  entityIDMap[tileWidthIndex + 1][tileLengthIndex - 1],
                  name);
    }
    else if (lengthLocation <= 0 &&
             widthLocation  <= 0) {

        paintTile(tileWidth,
                  tileLength,
                  entityIDMap[tileWidthIndex - 1][tileLengthIndex - 1],
                  name);

    }
    else if (lengthLocation >= tileLength &&
             widthLocation  <= 0) {

        paintTile(tileWidth,
                  0,
                  entityIDMap[tileWidthIndex - 1][tileLengthIndex + 1],
                  name);
    }
}

void generateScene(std::string sceneName) {

    // Utilize the terminal's ability to create and edit fbx files...
    // I know this should be lifted to a generic wrapper that edits the 
    // fbx but for now I will inject string commands to the terminal interface.

    Terminal* terminal = Terminal::instance();

    srand(time(NULL));

    const std::vector<Entity*>* entityList = nullptr;

    for (int tileIndexW = -halfWidthTiles; tileIndexW <= halfWidthTiles; tileIndexW++) {
        for (int tileIndexL = -halfLengthTiles; tileIndexL <= halfLengthTiles; tileIndexL++) {

            // First add a tile
            std::string command = "ADDTILE " + sceneName + " FOREST ";
            command            += std::to_string(tileIndexW * tileWidth)   + " ";
            command            += std::to_string(0)                            + " "; // Keep height 0 for now
            command            += std::to_string(tileIndexL * tileLength) + " ";
            command            += std::to_string(1)                            + " "; // w component for scaling
            command            += "SNOW.JPG DIRT.JPG ROCKS.JPG GRASS.JPG";
            terminal->processCommand(command);

            // Get latest entity list based on the previous addition of a tile which is guaranteed to be at the end :)
            entityList   = EngineManager::getEntityList();
            // Stores an entityID for every tile
            entityIDMap[tileIndexW + halfWidthTiles][tileIndexL + halfLengthTiles] = entityList->back()->getID();

            for (int treeIndex = 0; treeIndex < numTreesToPlace; treeIndex++) {

                int treeWidthLocation  = ((rand() % tileWidth));
                int treeLengthLocation = ((rand() % tileLength));
                treeWidthLocation     -= treeWidthLocation  % pathPixelDiameter;
                treeLengthLocation    -= treeLengthLocation % pathPixelDiameter;

                int tileWidthMin       = (tileIndexW * tileWidth)  - tileHalfWidth;
                int tileLengthMin      = (tileIndexL * tileLength) - tileHalfLength;

                // Flag grid location as having an item here
                tileGridFlag[(tileWidthMin  + treeWidthLocation  + (widthOfWorld  / 2)) / pathPixelDiameter]
                            [(tileLengthMin + treeLengthLocation + (lengthOfWorld / 2)) / pathPixelDiameter] = Item;

                // Add models around the path
                // The selection of trees we have are tree3, tree7 and tree8 for the time being
                std::string command  = "ADD " + sceneName + " ";
                int         treeType = rand() % typesOfTrees;

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

                // Rotate tree around Y axis
                float yRot  = static_cast<float>(rand() % 360);

                // Place to the left or right of the path, etc.
                command += std::to_string(tileWidthMin  + treeWidthLocation)  + " ";
                command += std::to_string(0)                                  + " "; // Keep height 0 for now
                command += std::to_string(tileLengthMin + treeLengthLocation) + " ";
                command += std::to_string(scale)                              + " "; // w component for scaling
                command += std::to_string(0)                                  + " ";
                command += std::to_string(yRot)                               + " "; // rotation around y
                command += std::to_string(0)                                  + " ";
                terminal->processCommand(command);
            }
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

void updatePath(std::string sceneName) {

    if (proceduralGenDone) {
        return;
    }

    // Utilize the terminal's ability to create and edit fbx files...
    // I know this should be lifted to a generic wrapper that edits the 
    // fbx but for now I will inject string commands to the terminal interface.
    Terminal* terminal  = Terminal::instance();

    srand(time(NULL));

    // Second add paths and terrain painting of the tile
    // and make sure the path is within the extents of the tile terrain texture
    if ((tileWidthIndex  >= 0)             &&
        (tileWidthIndex  <  numWidthTiles) &&
        (tileLengthIndex >= 0)             &&
        (tileLengthIndex <  numLengthTiles)) {

        if ((pathWidthLocation  >= 0)         &&
            (pathWidthLocation  <= tileWidth) &&
            (pathLengthLocation >= 0)         &&
            (pathLengthLocation <= tileLength)) {

            bool foundLocationWithoutPath = false;
            unsigned int directionFlags   = 0;
            while (!foundLocationWithoutPath) {

                // Find the next path location
                TileDirection nextPathDirection =
                    static_cast<TileDirection>(rand() % TileDirection::TileDirectionLength);

                int proposedPathWidthLocation  = pathWidthLocation;
                int proposedPathLengthLocation = pathLengthLocation;

                switch (nextPathDirection) {
                    case North:
                    {
                        proposedPathLengthLocation += pathPixelDiameter;
                        break;
                    }
                    case East:
                    {
                        proposedPathWidthLocation  += pathPixelDiameter;
                        break;
                    }
                    case South:
                    {
                        proposedPathLengthLocation -= pathPixelDiameter;
                        break;
                    }
                    case West:
                    {
                        proposedPathWidthLocation  -= pathPixelDiameter;
                        break;
                    }
                    case NorthEast:
                    {
                        proposedPathWidthLocation  += pathPixelDiameter;
                        proposedPathLengthLocation += pathPixelDiameter;
                        break;
                    }
                    case SouthEast:
                    {
                        proposedPathWidthLocation  += pathPixelDiameter;
                        proposedPathLengthLocation -= pathPixelDiameter;
                        break;
                    }
                    case NorthWest:
                    {
                        proposedPathWidthLocation  -= pathPixelDiameter;
                        proposedPathLengthLocation += pathPixelDiameter;
                        break;
                    }
                    case SouthWest:
                    {
                        proposedPathWidthLocation  -= pathPixelDiameter;
                        proposedPathLengthLocation -= pathPixelDiameter;
                        break;
                    }
                };

                // If every direction has been taken and no paths are taken then the path is stuck so quit early
                directionFlags |=
                    static_cast<unsigned int>(pow(2.0f, static_cast<float>(nextPathDirection)));
                if (directionFlags == static_cast<unsigned int>(pow(2.0f, static_cast<float>(TileDirection::TileDirectionLength)) - 1)) {
                    terminal->processCommand("SAVE SPAWN-TEST");
                    proceduralGenDone = true;
                    return;
                }

                // Flag grid location as having a path here
                TileFlag tileGridValue =
                    tileGridFlag[((tileWidthIndex  * tileWidth)  + proposedPathWidthLocation)  / pathPixelDiameter]
                                [((tileLengthIndex * tileLength) + proposedPathLengthLocation) / pathPixelDiameter];

                int tempDirection[2] = { proposedPathWidthLocation  - pathWidthLocation,
                                         proposedPathLengthLocation - pathLengthLocation };

                int  sign            = (direction[0] * tempDirection[0]) + (direction[1] * tempDirection[1]);

                if ((tileGridValue != Path) &&
                    (tileGridValue != Item)/* &&
                    (sign          >= 0.0f)*/) {

                    int prevEntityID = entityIDMap[prevTileWidthIndex][prevTileLengthIndex];

                    // DEBUG PATH CODE
                    // Paint previous path texture with the third texture option to indicate already traveled
                    // Send a middle mouse button click to change texture
                    terminal->processCommand("MOUSEPATHING 2 1 0 0 0");

                    paintTiles(prevPathWidthLocation,
                               prevPathLengthLocation,
                               prevTileWidthIndex,
                               prevTileLengthIndex,
                               sceneName);

                    //Reset texture back to path texture
                    terminal->processCommand("MOUSEPATHING 2 1 0 0 0");
                    terminal->processCommand("MOUSEPATHING 2 1 0 0 0");
                    terminal->processCommand("MOUSEPATHING 2 1 0 0 0");
                    // FINISHED DEBUG PATH CODE

                    pathWidthLocation        = proposedPathWidthLocation;
                    pathLengthLocation       = proposedPathLengthLocation;
                    foundLocationWithoutPath = true;
                    direction[0]             = tempDirection[0];
                    direction[1]             = tempDirection[1];

                    if (pathWidthLocation <= 0) {
                        tileWidthIndex--;
                        // Location when jumping into another tile
                        pathWidthLocation = tileWidth;
                    }
                    else if (pathWidthLocation >= tileWidth) {
                        tileWidthIndex++;
                        // Location when jumping into another tile
                        pathWidthLocation = 0;
                    }
                    if (pathLengthLocation <= 0) {
                        tileLengthIndex--;
                        // Location when jumping into another tile
                        pathLengthLocation = tileLength;
                    }
                    else if (pathLengthLocation >= tileLength) {
                        tileLengthIndex++;
                        // Location when jumping into another tile
                        pathLengthLocation = 0;
                    }

                    // Draw the current tile
                    paintTiles(pathWidthLocation,
                               pathLengthLocation,
                               tileWidthIndex,
                               tileLengthIndex,
                               sceneName);

                    prevPathWidthLocation  = pathWidthLocation;
                    prevPathLengthLocation = pathLengthLocation;
                    prevTileWidthIndex     = tileWidthIndex;
                    prevTileLengthIndex    = tileLengthIndex;

                    // Flag grid location as having a path here
                    tileGridFlag[((tileWidthIndex  * tileWidth)  + pathWidthLocation)  / pathPixelDiameter]
                                [((tileLengthIndex * tileLength) + pathLengthLocation) / pathPixelDiameter] = Path;
                }
            }

            //// Chance of placing a tree near the path
            //bool placingItem = (rand() % probabilityToPlaceItem == 0) ? true : false;
            //if (placingItem == true) {

            //    // Flag grid location as having an item here
            //    tileGridFlag[((tileWidthIndex  * tileWidth)  + pathWidthLocation - pathPixelDiameter) / pathPixelDiameter]
            //                [((tileLengthIndex * tileLength) + pathLengthLocation)                    / pathPixelDiameter] = Item;

            //    float tileWidthMin  = ((tileWidthIndex  - halfWidthTiles)  * tileWidth)  - tileHalfWidth;
            //    float tileLengthMin = ((tileLengthIndex - halfLengthTiles) * tileLength) - tileHalfLength;

            //    // Add models around the path
            //    // The selection of trees we have are tree3, tree7 and tree8 for the time being
            //    std::string command  = "ADD " + sceneName + " ";
            //    int treeType = rand() % typesOfTrees;

            //    // Select tree type
            //    if (treeType == 0) {
            //        command += "TREE3 ";
            //    }
            //    else if (treeType == 1) {
            //        command += "TREE7 ";
            //    }
            //    else if (treeType == 2) {
            //        command += "TREE8 ";
            //    }

            //    // Scale model between 0.25 and 0.75
            //    float scale = ((static_cast<float>(rand()) / maxRandomValue) * 0.5f) + 0.25f;

            //    // Place to the left or right of the path, etc.
            //    command += std::to_string(tileWidthMin  + pathWidthLocation - pathPixelDiameter) + " ";
            //    command += std::to_string(0)                                                     + " "; // Keep height 0 for now
            //    command += std::to_string(tileLengthMin + pathLengthLocation)                    + " ";
            //    command += std::to_string(scale)                                                 + " "; // w component for scaling

            //    terminal->processCommand(command);
            //}
        }
    }
    else {
        terminal->processCommand("SAVE SPAWN-TEST");
        proceduralGenDone = true;
    }
}
