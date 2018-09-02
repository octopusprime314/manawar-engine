#include "Matrix.h"
#include "Vector4.h"
#include "IOEventDistributor.h"
#include <iostream>
#include <thread>
#include <algorithm>
#include "EngineManager.h"
#include "Logger.h"

int main(int argc, char** argv) {

	// Logger command line settings
	// Doesn't seem like there is any real CLI handling so i'll toss it here
	for (int i = 1; i < argc; i++) {
		std::string arg(argv[i]);
		std::cout << arg << std::endl;
		if ( arg == LOGGERCLI ) {
			if ( argc >= (i + 1) ) {
				std::string log_level(argv[i + 1]);
				std::transform(log_level.begin(), log_level.end(), log_level.begin(), ::tolower);
				Logger::SetLogLevel(log_level);
			}
		}
	}

    //Send the width and height in pixel units and the near and far plane to describe the view frustum
    EngineManager EngineManager(&argc, argv); //Manages the camera view and models in scene
	Logger::CloseLog();
    return 0;
}

int CALLBACK WinMain(
    _In_ HINSTANCE hInstance,
    _In_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
)
{
    char argv0[] = "Reboot.exe";
    char* argv[] = { argv0 };
    main(1, argv);
}
