// For laptops running a discrete and integrated graphics card, these symbols
// tell the driver to use the discrete card over the integrated one.
// As far as I know, this only happens on Windows.
#ifdef WIN32

#include <windows.h>

extern "C" _declspec(dllexport) DWORD NvOptimusEnablement                  = 1;
extern "C" _declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 1;

#endif
