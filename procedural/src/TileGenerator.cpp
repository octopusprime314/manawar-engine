#include "TileGenerator.h"
#include "Terminal.h"
#include <iostream>

void generateScene(std::string sceneName) {

    // Utilize the terminal's ability to create and edit fbx files...
    // I know this should be lifted to a generic wrapper that edits the 
    // fbx but for now I will inject string commands to the terminal interface.

    Terminal* terminal = Terminal::instance();

    //terminal->processCommand("CREATE SPAWN-TEST");

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
    const int   numTreesToPlace = 10;
    const float maxRandomValue  = static_cast<float>(RAND_MAX);

    // Establish a seed for the random number generator
    srand(time(NULL));

    for (int tileWidthIndex = -halfWidthTiles; tileWidthIndex < halfWidthTiles; tileWidthIndex++) {
        for (int tileLengthIndex = -halfLengthTiles; tileLengthIndex < halfLengthTiles; tileLengthIndex++) {

            std::string command = "ADDTILE " + sceneName + " FOREST ";
            command            += std::to_string(tileWidthIndex * tileWidth)   + " ";
            command            += std::to_string(0)                            + " "; // Keep height 0 for now
            command            += std::to_string(tileLengthIndex * tileLength) + " ";
            command            += std::to_string(1)                            + " "; // w component for scaling
            command            += "SNOW.JPG DIRT.JPG ROCKS.JPG GRASS.JPG";
            terminal->processCommand(command);


            // Add 10 random types of trees within random locations of the tile
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

                float tileWidthMin  = (tileWidthIndex  * tileWidth)  - (tileHalfWidth);
                float tileWidthMax  = (tileWidthIndex  * tileWidth)  + (tileHalfWidth);
                float tileLengthMin = (tileLengthIndex * tileLength) - (tileHalfLength);
                float tileLengthMax = (tileLengthIndex * tileLength) + (tileHalfLength);

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
                //terminal->processCommand("ADD SPAWN-TEST TREE3 0.0 0.0 0.0 0.125");
            }

        }
    }
    terminal->processCommand("SAVE SPAWN-TEST");

    // Test commands
    //terminal->processCommand("ADDTILE SPAWN-TEST FOREST 0 0 0 1 SNOW.JPG DIRT.JPG ROCKS.JPG GRASS.JPG");
    //terminal->processCommand("ADDTILE SPAWN-TEST SWAMP 0 0 200 1 SNOW.JPG DIRT.JPG ROCKS.JPG GRASS.JPG");
    //terminal->processCommand("ADDTILE SPAWN-TEST PLAINS 0 0 -200 1 SNOW.JPG DIRT.JPG ROCKS.JPG GRASS.JPG");
    //terminal->processCommand("ADDTILE SPAWN-TEST MOUNTAIN 200 0 0 1 SNOW.JPG DIRT.JPG ROCKS.JPG GRASS.JPG");
    //terminal->processCommand("ADDTILE SPAWN-TEST ISLAND -200 0 0 1 SNOW.JPG DIRT.JPG ROCKS.JPG GRASS.JPG");
    //terminal->processCommand("ADD SPAWN-TEST TREE3 0.0 0.0 0.0 0.125");
}
