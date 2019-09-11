#include "TileGenerator.h"
#include "Terminal.h"
#include "EngineManager.h"
#include <iostream>

void generateScene(std::string sceneName) {

    // Utilize the terminal's ability to create and edit fbx files...
    // I know this should be lifted to a generic wrapper that edits the 
    // fbx but for now I will inject string commands to the terminal interface.

    Terminal* terminal = Terminal::instance();

    // First define the extents of the world and use the hardcoded tile size of 200x200
    const int   tileWidth       = 200;
    const int   tileLength      = 200;
    const int   tileHalfWidth   = tileWidth  / 2;
    const int   tileHalfLength  = tileLength / 2;
    const int   widthOfWorld    = 1000;
    const int   lengthOfWorld   = 1000;
    const int   heightOfWorld   = 1000;
    const int   numWidthTiles   = widthOfWorld   / tileWidth;
    const int   numLengthTiles  = lengthOfWorld  / tileLength;
    const int   halfWidthTiles  = numWidthTiles  / 2;
    const int   halfLengthTiles = numLengthTiles / 2;
    const int   typesOfTrees    = 3;
    const int   numTreesToPlace = 5;
    const float maxRandomValue  = static_cast<float>(RAND_MAX);

    const std::vector<Entity*>* entityList = nullptr;
    
    // Establish a seed for the random number generator
    srand(time(NULL));

    // Small temporary hack that sets the texture for pathing and painting terrain to non background texture
    // Send a middle mouse button click to change texture
    terminal->processCommand("MOUSEPATHING 2 1 0 0 0");

    for (int tileWidthIndex = -halfWidthTiles; tileWidthIndex < halfWidthTiles; tileWidthIndex++) {
        for (int tileLengthIndex = -halfLengthTiles; tileLengthIndex < halfLengthTiles; tileLengthIndex++) {

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
            int entityID = entityList->back()->getID();

            // Second add paths and terrain painting of the tile
            // and make sure the path is within the extents of the tile terrain texture
            int pathWidthLocation  = tileHalfWidth;
            int pathLengthLocation = tileHalfLength;

            while ((pathWidthLocation  >= 0)         &&
                   (pathWidthLocation  <  tileWidth) &&
                   (pathLengthLocation >= 0)         &&
                   (pathLengthLocation <  tileLength)) {

                // Left mouse click command
                command = "MOUSEPATHING 0 1 ";

                command += std::to_string(pathWidthLocation)  + " ";
                command += std::to_string(pathLengthLocation) + " ";
                command += std::to_string(entityID);

                terminal->processCommand(command);

                TileDirection nextPathLocationWidth  = static_cast<TileDirection>(rand() % TileDirection::TileDirectionLength);
                TileDirection nextPathLocationLength = static_cast<TileDirection>(rand() % TileDirection::TileDirectionLength);

                for (int pathPass = 0; pathPass < 2; pathPass++) {
                    TileDirection direction = pathPass ? nextPathLocationLength : nextPathLocationWidth;

                    switch (direction) {
                        case Left: 
                        {
                            if (pathPass == 0) {
                                pathWidthLocation--;
                            }
                            break;
                        }
                        case Right:
                        {
                            if (pathPass == 0) {
                                pathWidthLocation++;
                            }
                            break;
                        }
                        case Up:
                        {
                            if (pathPass == 1) {
                                pathLengthLocation--;
                            }
                            break;
                        }
                        case Down:
                        {
                            if (pathPass == 1) {
                                pathLengthLocation++;
                            }
                            break;
                        }
                        case None: 
                        {
                            break;
                        }
                    };
                }
            }

            float tileWidthMin  = (tileWidthIndex  * tileWidth)  - tileHalfWidth;
            float tileLengthMin = (tileLengthIndex * tileLength) - tileHalfLength;

            // Third add models to the tile
            // Add random types of trees within random locations of the tile
            // The selection of trees we have are tree3, tree7 and tree8 for the time being
            for (int treeIndex = 0; treeIndex < numTreesToPlace; treeIndex++) {

                command = "ADD " + sceneName + " ";

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

                // Place between the min and max extents of the width and length of the tile
                float widthScale    = (static_cast<float>(rand()) / maxRandomValue);
                float lengthScale   = (static_cast<float>(rand()) / maxRandomValue);
                // Scale model between 0.25 and 0.75
                float scale         = ((static_cast<float>(rand()) / maxRandomValue) * 0.5f) + 0.25f;

                command += std::to_string(tileWidthMin  + (widthScale  * tileWidth))  + " ";
                command += std::to_string(0)                                          + " "; // Keep height 0 for now
                command += std::to_string(tileLengthMin + (lengthScale * tileLength)) + " ";
                command += std::to_string(scale)                                      + " "; // w component for scaling

                terminal->processCommand(command);
            }
        }
    }
    terminal->processCommand("SAVE SPAWN-TEST");
}
