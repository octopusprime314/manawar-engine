#include "TileGenerator.h"
#include "Terminal.h"

void generateScene(std::string sceneName) {

    // Utilize the terminal's ability to create and edit fbx files...
    // I know this should be lifted to a generic wrapper that edits the 
    // fbx but for now I will inject string commands to the terminal interface.

    Terminal* terminal = Terminal::instance();

    //terminal->processCommand("CREATE SPAWN-TEST");
    terminal->processCommand("ADDTILE SPAWN-TEST FOREST 0 0 0 1 SNOW.JPG DIRT.JPG ROCKS.JPG GRASS.JPG");
    terminal->processCommand("ADDTILE SPAWN-TEST SWAMP 0 0 200 1 SNOW.JPG DIRT.JPG ROCKS.JPG GRASS.JPG");
    terminal->processCommand("ADDTILE SPAWN-TEST PLAINS 0 0 -200 1 SNOW.JPG DIRT.JPG ROCKS.JPG GRASS.JPG");
    terminal->processCommand("ADDTILE SPAWN-TEST MOUNTAIN 200 0 0 1 SNOW.JPG DIRT.JPG ROCKS.JPG GRASS.JPG");
    terminal->processCommand("ADDTILE SPAWN-TEST ISLAND -200 0 0 1 SNOW.JPG DIRT.JPG ROCKS.JPG GRASS.JPG");
    terminal->processCommand("ADD SPAWN-TEST TREE3 0.0 0.0 0.0 0.125");
    terminal->processCommand("SAVE SPAWN-TEST");
}
