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

const int probabilityToPlaceItem = 60; // 1/60
int       pathWidthLocation      = tileHalfWidth;
int       pathLengthLocation     = tileHalfLength;
int       tileWidthIndex         = numWidthTiles  / 2;
int       tileLengthIndex        = numLengthTiles / 2;
int       direction[2]           = {0, 0};
bool      proceduralGenDone      = false;


// Identifies a tile within the grid's entity ID for access
int entityIDMap[numWidthTiles][numLengthTiles];

// Flags used to identify what type of object is at this location in the grid
TileFlag tileGridFlag[widthOfWorld / pathPixelRadius][lengthOfWorld / pathPixelRadius];

void generateScene(std::string sceneName) {

    // Utilize the terminal's ability to create and edit fbx files...
    // I know this should be lifted to a generic wrapper that edits the 
    // fbx but for now I will inject string commands to the terminal interface.

    Terminal* terminal = Terminal::instance();

    const std::vector<Entity*>* entityList = nullptr;

    // Establish a seed for the random number generator
    srand(time(NULL));

    // Prevents paths from going in drastically different directions
    direction[0] = (rand() % 2) ? 10 : -10;
    direction[1] = (rand() % 2) ? 10 : -10;

    for (int tileWidthIndex = -halfWidthTiles; tileWidthIndex <= halfWidthTiles; tileWidthIndex++) {
        for (int tileLengthIndex = -halfLengthTiles; tileLengthIndex <= halfLengthTiles; tileLengthIndex++) {

            // First add a tile
            std::string command = "ADDTILE " + sceneName + " FOREST ";
            command            += std::to_string(tileWidthIndex * tileWidth)   + " ";
            command            += std::to_string(0)                            + " "; // Keep height 0 for now
            command            += std::to_string(tileLengthIndex * tileLength) + " ";
            command            += std::to_string(1)                            + " "; // w component for scaling
            command            += "SNOW.JPG DIRT.JPG ROCKS.JPG GRASS.JPG";
            terminal->processCommand(command);

            // Get latest entity list based on the previous addition of a tile which is guaranteed to be at the end :)
            entityList   = EngineManager::getEntityList();
            // Stores an entityID for every tile
            entityIDMap[tileWidthIndex + halfWidthTiles][tileLengthIndex + halfLengthTiles] = entityList->back()->getID();


            //// Third add models to the tile
            //// Add random types of trees within random locations of the tile
            //// The selection of trees we have are tree3, tree7 and tree8 for the time being
            // float tileWidthMin  = (tileWidthIndex  * tileWidth)  - tileHalfWidth;
            // float tileLengthMin = (tileLengthIndex * tileLength) - tileHalfLength;
            //for (int treeIndex = 0; treeIndex < numTreesToPlace; treeIndex++) {

            //    command = "ADD " + sceneName + " ";

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

            //    // Place between the min and max extents of the width and length of the tile
            //    float widthScale    = (static_cast<float>(rand()) / maxRandomValue);
            //    float lengthScale   = (static_cast<float>(rand()) / maxRandomValue);
            //    // Scale model between 0.25 and 0.75
            //    float scale         = ((static_cast<float>(rand()) / maxRandomValue) * 0.5f) + 0.25f;

            //    command += std::to_string(tileWidthMin  + (widthScale  * tileWidth))  + " ";
            //    command += std::to_string(0)                                          + " "; // Keep height 0 for now
            //    command += std::to_string(tileLengthMin + (lengthScale * tileLength)) + " ";
            //    command += std::to_string(scale)                                      + " "; // w component for scaling

            //    terminal->processCommand(command);
            //}
        }
    }

    // Small temporary hack that sets the texture for pathing and painting terrain to non background texture
    // Send a middle mouse button click to change texture
    terminal->processCommand("MOUSEPATHING 2 1 0 0 0");

    // Increase the radius of the texture painting
    for (int i = 0; i < pathPixelRadius; i++) {
        terminal->processCommand("INCREASE_PAINT_SIZE");
    }

    //// Second add paths and terrain painting of the tile
    //// and make sure the path is within the extents of the tile terrain texture
    //while ((tileWidthIndex  >= 0)             &&
    //       (tileWidthIndex  <  numWidthTiles) &&
    //       (tileLengthIndex >= 0)             &&
    //       (tileLengthIndex <  numLengthTiles)) {

    //    int entityID = entityIDMap[tileWidthIndex][tileLengthIndex];

    //    while ((pathWidthLocation  >= 0)         &&
    //           (pathWidthLocation  <  tileWidth) &&
    //           (pathLengthLocation >= 0)         &&
    //           (pathLengthLocation <  tileLength)) {

    //        // Left mouse click command
    //        std::string command = "MOUSEPATHING 0 1 ";
    //        command += std::to_string(pathWidthLocation)  + " ";
    //        command += std::to_string(pathLengthLocation) + " ";
    //        command += std::to_string(entityID);

    //        terminal->processCommand(command);

    //        // Flag grid location as having a path here
    //        tileGridFlag[((tileWidthIndex  * tileWidth)  + pathWidthLocation)  / pathPixelRadius]
    //                    [((tileLengthIndex * tileLength) + pathLengthLocation) / pathPixelRadius] = Path;

    //        // Chance of placing a tree near the path
    //        bool placingItem = (rand() % probabilityToPlaceItem == 0) ? true : false;
    //        if (placingItem == true) {

    //            // Flag grid location as having an item here
    //            tileGridFlag[((tileWidthIndex  * tileWidth)  + pathWidthLocation - pathPixelRadius)  / pathPixelRadius]
    //                        [((tileLengthIndex * tileLength) + pathLengthLocation)                   / pathPixelRadius] = Item;

    //            float tileWidthMin  = ((tileWidthIndex  - halfWidthTiles)  * tileWidth)  - tileHalfWidth;
    //            float tileLengthMin = ((tileLengthIndex - halfLengthTiles) * tileLength) - tileHalfLength;

    //            // Add models around the path
    //            // The selection of trees we have are tree3, tree7 and tree8 for the time being
    //            command      = "ADD " + sceneName + " ";
    //            int treeType = rand() % typesOfTrees;

    //            // Select tree type
    //            if (treeType == 0) {
    //                command += "TREE3 ";
    //            }
    //            else if (treeType == 1) {
    //                command += "TREE7 ";
    //            }
    //            else if (treeType == 2) {
    //                command += "TREE8 ";
    //            }

    //            // Scale model between 0.25 and 0.75
    //            float scale = ((static_cast<float>(rand()) / maxRandomValue) * 0.5f) + 0.25f;

    //            // Place to the left or right of the path, etc.
    //            command += std::to_string(tileWidthMin  + pathWidthLocation - pathPixelRadius) + " ";
    //            command += std::to_string(0)                                                   + " "; // Keep height 0 for now
    //            command += std::to_string(tileLengthMin + pathLengthLocation)                  + " ";
    //            command += std::to_string(scale)                                               + " "; // w component for scaling

    //            terminal->processCommand(command);
    //        }

    //        bool foundLocationWithoutPath = false;
    //        while (!foundLocationWithoutPath) {

    //            // Find the next path location
    //            TileDirection nextPathLocationWidth =
    //                static_cast<TileDirection>(rand() % TileDirection::TileDirectionLength);
    //            TileDirection nextPathLocationLength =
    //                static_cast<TileDirection>(rand() % TileDirection::TileDirectionLength);

    //            int proposedPathWidthLocation  = pathWidthLocation;
    //            int proposedPathLengthLocation = pathLengthLocation;

    //            for (int pathPass = 0; pathPass < 2; pathPass++) {
    //                TileDirection direction = pathPass ? nextPathLocationLength : nextPathLocationWidth;

    //                switch (direction) {
    //                    case Left:
    //                    {
    //                        if (pathPass == 0) {
    //                            proposedPathWidthLocation -= pathPixelRadius;
    //                        }
    //                        break;
    //                    }
    //                    case Right:
    //                    {
    //                        if (pathPass == 0) {
    //                            proposedPathWidthLocation += pathPixelRadius;
    //                        }
    //                        break;
    //                    }
    //                    case Up:
    //                    {
    //                        if (pathPass == 1) {
    //                            proposedPathLengthLocation -= pathPixelRadius;
    //                        }
    //                        break;
    //                    }
    //                    case Down:
    //                    {
    //                        if (pathPass == 1) {
    //                            proposedPathLengthLocation += pathPixelRadius;
    //                        }
    //                        break;
    //                    }
    //                    case None: 
    //                    {
    //                        break;
    //                    }
    //                };
    //            }
    //            // Flag grid location as having a path here
    //            TileFlag tileGridValue =
    //                tileGridFlag[((tileWidthIndex  * tileWidth)  + proposedPathWidthLocation)  / pathPixelRadius]
    //                            [((tileLengthIndex * tileLength) + proposedPathLengthLocation) / pathPixelRadius];

    //            int tempDirection[2] = { proposedPathWidthLocation  - pathWidthLocation,
    //                                     proposedPathLengthLocation - pathLengthLocation };

    //            int  sign            = (direction[0] * tempDirection[0]) + (direction[1] * tempDirection[1]);

    //            if (tileGridValue != Path &&
    //                tileGridValue != Item &&
    //                sign          >= 0.0f) {
    //                pathWidthLocation        = proposedPathWidthLocation;
    //                pathLengthLocation       = proposedPathLengthLocation;
    //                foundLocationWithoutPath = true;
    //                direction[0]             = tempDirection[0];
    //                direction[1]             = tempDirection[1];
    //            }
    //        }
    //    }

    //    if (pathWidthLocation < 0) {
    //        tileWidthIndex--;
    //        // Location when jumping into another tile
    //        pathWidthLocation = tileWidth - pathPixelRadius;
    //    }
    //    if (pathWidthLocation >= tileWidth) {
    //        tileWidthIndex++;
    //        // Location when jumping into another tile
    //        pathWidthLocation = 0;
    //    }
    //    if (pathLengthLocation < 0) {
    //        tileLengthIndex--;
    //        // Location when jumping into another tile
    //        pathLengthLocation = tileLength - pathPixelRadius;
    //    }
    //    if (pathLengthLocation >= tileLength) {
    //        tileLengthIndex++;
    //        // Location when jumping into another tile
    //        pathLengthLocation = 0;
    //    }
    //}
    //terminal->processCommand("SAVE SPAWN-TEST");
}

void updatePath(std::string sceneName) {

    if (proceduralGenDone) {
        return;
    }

    // Utilize the terminal's ability to create and edit fbx files...
    // I know this should be lifted to a generic wrapper that edits the 
    // fbx but for now I will inject string commands to the terminal interface.

    Terminal* terminal  = Terminal::instance();

    // Second add paths and terrain painting of the tile
    // and make sure the path is within the extents of the tile terrain texture
    if ((tileWidthIndex  >= 0)             &&
        (tileWidthIndex  <  numWidthTiles) &&
        (tileLengthIndex >= 0)             &&
        (tileLengthIndex <  numLengthTiles)) {

        int entityID = entityIDMap[tileWidthIndex][tileLengthIndex];

        if ((pathWidthLocation  >= 0)         &&
            (pathWidthLocation  <  tileWidth) &&
            (pathLengthLocation >= 0)         &&
            (pathLengthLocation <  tileLength)) {

            // Left mouse click command
            std::string command = "MOUSEPATHING 0 1 ";
            command += std::to_string(pathWidthLocation)  + " ";
            command += std::to_string(pathLengthLocation) + " ";
            command += std::to_string(entityID)           + " ";
            command += sceneName;

            terminal->processCommand(command);

            // Flag grid location as having a path here
            tileGridFlag[((tileWidthIndex  * tileWidth)  + pathWidthLocation)  / pathPixelRadius]
                        [((tileLengthIndex * tileLength) + pathLengthLocation) / pathPixelRadius] = Path;

            // Chance of placing a tree near the path
            bool placingItem = (rand() % probabilityToPlaceItem == 0) ? true : false;
            if (placingItem == true) {

                // Flag grid location as having an item here
                tileGridFlag[((tileWidthIndex  * tileWidth)  + pathWidthLocation - pathPixelRadius)  / pathPixelRadius]
                            [((tileLengthIndex * tileLength) + pathLengthLocation)                   / pathPixelRadius] = Item;

                float tileWidthMin  = ((tileWidthIndex  - halfWidthTiles)  * tileWidth)  - tileHalfWidth;
                float tileLengthMin = ((tileLengthIndex - halfLengthTiles) * tileLength) - tileHalfLength;

                // Add models around the path
                // The selection of trees we have are tree3, tree7 and tree8 for the time being
                command      = "ADD " + sceneName + " ";
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
                command += std::to_string(tileWidthMin  + pathWidthLocation - pathPixelRadius) + " ";
                command += std::to_string(0)                                                   + " "; // Keep height 0 for now
                command += std::to_string(tileLengthMin + pathLengthLocation)                  + " ";
                command += std::to_string(scale)                                               + " "; // w component for scaling

                terminal->processCommand(command);
            }

            bool foundLocationWithoutPath = false;
            while (!foundLocationWithoutPath) {

                // Find the next path location
                TileDirection nextPathLocationWidth =
                    static_cast<TileDirection>(rand() % TileDirection::TileDirectionLength);
                TileDirection nextPathLocationLength =
                    static_cast<TileDirection>(rand() % TileDirection::TileDirectionLength);

                int proposedPathWidthLocation  = pathWidthLocation;
                int proposedPathLengthLocation = pathLengthLocation;

                for (int pathPass = 0; pathPass < 2; pathPass++) {
                    TileDirection direction = pathPass ? nextPathLocationLength : nextPathLocationWidth;

                    switch (direction) {
                        case Left:
                        {
                            if (pathPass == 0) {
                                proposedPathWidthLocation -= pathPixelRadius;
                            }
                            break;
                        }
                        case Right:
                        {
                            if (pathPass == 0) {
                                proposedPathWidthLocation += pathPixelRadius;
                            }
                            break;
                        }
                        case Up:
                        {
                            if (pathPass == 1) {
                                proposedPathLengthLocation -= pathPixelRadius;
                            }
                            break;
                        }
                        case Down:
                        {
                            if (pathPass == 1) {
                                proposedPathLengthLocation += pathPixelRadius;
                            }
                            break;
                        }
                        case None: 
                        {
                            break;
                        }
                    };
                }
                // Flag grid location as having a path here
                TileFlag tileGridValue =
                    tileGridFlag[((tileWidthIndex  * tileWidth)  + proposedPathWidthLocation)  / pathPixelRadius]
                                [((tileLengthIndex * tileLength) + proposedPathLengthLocation) / pathPixelRadius];

                int tempDirection[2] = { proposedPathWidthLocation  - pathWidthLocation,
                                         proposedPathLengthLocation - pathLengthLocation };

                int  sign            = (direction[0] * tempDirection[0]) + (direction[1] * tempDirection[1]);

                if (/*(tileGridValue != Path) &&
                    (tileGridValue != Item) &&*/
                    (sign          >= 0.0f)) {
                    pathWidthLocation        = proposedPathWidthLocation;
                    pathLengthLocation       = proposedPathLengthLocation;
                    foundLocationWithoutPath = true;
                    direction[0]             = tempDirection[0];
                    direction[1]             = tempDirection[1];
                }
            }
        }

        if (pathWidthLocation < 0) {
            tileWidthIndex--;
            // Location when jumping into another tile
            pathWidthLocation = tileWidth - pathPixelRadius;
        }
        if (pathWidthLocation >= tileWidth) {
            tileWidthIndex++;
            // Location when jumping into another tile
            pathWidthLocation = 0;
        }
        if (pathLengthLocation < 0) {
            tileLengthIndex--;
            // Location when jumping into another tile
            pathLengthLocation = tileLength - pathPixelRadius;
        }
        if (pathLengthLocation >= tileLength) {
            tileLengthIndex++;
            // Location when jumping into another tile
            pathLengthLocation = 0;
        }
    }
    else {
        terminal->processCommand("SAVE SPAWN-TEST");
        proceduralGenDone = true;
    }
}
