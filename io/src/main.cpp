#include "Matrix.h"
#include "Vector4.h"
#include "IOEventDistributor.h"
#include <iostream>
#include <thread>
#include <algorithm>
#include "EngineManager.h"
#include "Logger.h"
#include <Windows.h>

int main_entry(int       argc,
               char**    argv,
               HINSTANCE hInstance,
               int       nCmdShow) {

    // Logger command line settings
    // Doesn't seem like there is any real CLI handling so i'll toss it here
    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        std::cout << arg << std::endl;
        if ( arg == LOGGERCLI ) {
            if ( argc >= (i + 1) ) {
                std::string log_level(argv[i + 1]);
                std::transform(log_level.begin(), log_level.end(), log_level.begin(), ::tolower);
                Logger::setLogLevel(log_level);
            }
        }
    }

    //Send the width and height in pixel units and the near and far plane to describe the view frustum
    EngineManager EngineManager(&argc,
                                argv,
                                hInstance,
                                nCmdShow);
    Logger::closeLog();
    return 0;
}

int CALLBACK WinMain(_In_     HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_     LPTSTR    lpCmdLine,
                     _In_     int       nCmdShow) {

    char  argv0[] = "Reboot.exe";
    char* argv[]  = { argv0 };

    main_entry(1,
               argv,
               hInstance,
               nCmdShow);
    return 0;
}