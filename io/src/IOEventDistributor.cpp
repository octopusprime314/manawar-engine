#include "IOEventDistributor.h"
#include "ViewEvents.h"
#include "MasterClock.h"
#include "EngineManager.h"
#include "DXLayer.h"
#include "Windowsx.h"

// Events that trigger at a specific time
TimeQueue   IOEventDistributor::_timeEvents;
std::mutex  IOEventDistributor::_renderLock;
int         IOEventDistributor::_renderNow        = 0;
GLFWwindow* IOEventDistributor::_window           = nullptr;
bool        IOEventDistributor::_quit             = false;
int         IOEventDistributor::_prevMouseX       = 0;
int         IOEventDistributor::_prevMouseY       = 0;
int         IOEventDistributor::screenPixelWidth  = -1;
int         IOEventDistributor::screenPixelHeight = -1;

uint64_t nowMs() {
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;

    return duration_cast<milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

IOEventDistributor::IOEventDistributor(int* argc,
                                       char** argv,
                                       HINSTANCE hInstance,
                                       int nCmdShow) {

    char workingDir[1024] = "";
    GetCurrentDirectory(sizeof(workingDir), workingDir);
    std::cout << "Working directory: " << workingDir << std::endl;

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        //Initialize glfw for window creation
        glfwInit();

        //Make opengl core profile 4.3
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_MAXIMIZED, true);

        glfwSetErrorCallback([](int code, const char* pMsg) {
            std::cout << "GLFW " << code << " " << pMsg << std::endl;
        });

        const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        int monitorResoltuionY   = mode->height;
        int monitorResoltuionX   = mode->width;

        //WINDOWED MODE
        _window = glfwCreateWindow(monitorResoltuionX,
                                   monitorResoltuionY,
                                   "manawar-engine",
                                   NULL,
                                   NULL);
        //FULLSCREEN MODE
        //_window = glfwCreateWindow(monitorResoltuionX,
        //                           monitorResoltuionY,
        //                           "manawar-engine",
        //                           glfwGetPrimaryMonitor(),
        //                           NULL);
        if (!_window) {
            // Window or OpenGL context creation failed
            // The error callback above will tell us what happened.
            std::abort();
        }
        glfwMakeContextCurrent(_window); //Make current opengl context current

        int width, height;
        glfwGetWindowSize(_window, &width, &height);

        IOEventDistributor::screenPixelWidth  = width;
        IOEventDistributor::screenPixelHeight = height;

        //Callbacks
        glfwSetKeyCallback(        _window, &IOEventDistributor::_keyboardUpdate);
        glfwSetCursorPosCallback(  _window, &IOEventDistributor::_mouseUpdate);
        glfwSetMouseButtonCallback(_window, &IOEventDistributor::_mouseClick);

        //Sets atleast one extra render buffer for double buffering to prevent screen tearing
        //glfwSwapInterval(1); //Enables 60 hz vsync
        glfwSwapInterval(0); //Disables 60 hz vsync

        if (gl3wInit()) {
            std::cout << "failed to initialize OpenGL\n" << std::endl;
        }
        if (!gl3wIsSupported(3, 0)) {
            std::cout << "OpenGL 3.2 not supported\n" << std::endl;
        }
        printf("OpenGL: %s\nGLSL: %s\nVendor: %s\n",
               glGetString(GL_VERSION),
               glGetString(GL_SHADING_LANGUAGE_VERSION),
               glGetString(GL_VENDOR));

        //Depth buffer settings
        glClearDepth(1.0);
        glEnable(    GL_DEPTH_TEST);
        glDepthFunc( GL_LESS);
        //Color buffer settings
        glClearColor(0.0, 0.0, 0.0, 0.0);
        //Stencil buffer settings
        glClearStencil(0);

        //Disable mouse cursor view
        glfwSetInputMode(_window,
                         GLFW_CURSOR,
                         GLFW_CURSOR_DISABLED);
    }
    else {

    }

    _prevMouseX = IOEventDistributor::screenPixelWidth  / 2;
    _prevMouseY = IOEventDistributor::screenPixelHeight / 2;

    MasterClock* masterClock = MasterClock::instance();
    // Establishes the frame rate of the draw context
    // For some reason i need to double the framerate to get the target frame rate of 60????
    masterClock->setFrameRate(120); 
    masterClock->subscribeFrameRate(std::bind(_frameRateTrigger, std::placeholders::_1));

    // Hard coded debug events
    TimeEvent::Callback* debugCallback = []() { printf("%g\n", nowMs() / 1e3f); };
}

void IOEventDistributor::run() {
    _drawUpdate();
}

void IOEventDistributor::subscribeToKeyboard(       std::function<void(int, int, int)> func) {
    _events.subscribeToKeyboard(func);
}
void IOEventDistributor::subscribeToReleaseKeyboard(std::function<void(int, int, int)> func) {
    _events.subscribeToKeyboard(func);
}
void IOEventDistributor::subscribeToMouse(          std::function<void(double, double)> func) {
    _events.subscribeToMouse(func);
}
void IOEventDistributor::subscribeToDraw(           std::function<void()> func) {
    _events.subscribeToDraw(func);
}
void IOEventDistributor::subscribeToGameState(      std::function<void(EngineStateFlags)> func) {
    _events.subscribeToGameState(func);
}
void IOEventDistributor::updateGameState(           EngineStateFlags state) {
    IOEvents::updateGameState(state);
}

LRESULT CALLBACK IOEventDistributor::dxEventLoop(HWND hWnd,
                                                 UINT message,
                                                 WPARAM wParam,
                                                 LPARAM lParam) {

    switch (message)
    {
        case WM_KEYDOWN:
        {
            int key       = static_cast<int>(wParam);
            bool isRepeat = (lParam >> 30) & 0x1;
            if (!isRepeat) {
                _keyboardUpdate(nullptr, key, 0, 1, 0);
            }
            break;
        }
        case WM_KEYUP:
        {
            int key = static_cast<int>(wParam);
            _keyboardUpdate(nullptr, key, 0, 0, 0);
            break;
        }
        case WM_MOUSEMOVE:
        {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            if (_prevMouseX != IOEventDistributor::screenPixelWidth  / 2 &&
                _prevMouseY != IOEventDistributor::screenPixelHeight / 2) {

                _mouseUpdate(nullptr, xPos, yPos);
                //Indicates free form mouse update
                _prevMouseX = 0;
                _prevMouseX = 0;
            }
            break;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
        default:
            break;
    }

    // Handle any messages the switch statement didn't.
    return DefWindowProc(hWnd, message, wParam, lParam);
}

void IOEventDistributor::quit() {
    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        glfwDestroyWindow(_window);
        glfwTerminate();
    }
    else {

    }
    _quit = true;
    exit(0);
}

//All keyboard input from glfw will be notified here
void IOEventDistributor::_keyboardUpdate(GLFWwindow* window,
                                         int         key,
                                         int         scancode,
                                         int         action,
                                         int         mods) {

    if (action == GLFW_PRESS) {

        //Escape key pressed, hard exit no cleanup, TODO FIX THIS!!!!
        if (key == GLFW_KEY_ESCAPE ||
            key == 27) {
            
            if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
                glfwDestroyWindow(_window);
                glfwTerminate();
            }
            else {

            }
            _quit = true;
            exit(0);
        }

        //toggle between world editor and game mode
        if (key == GLFW_KEY_GRAVE_ACCENT) {
            EngineStateFlags engineStateFlags = EngineState::getEngineState();
            
            engineStateFlags.worldEditorModeEnabled = !engineStateFlags.worldEditorModeEnabled;
            engineStateFlags.gameModeEnabled = !engineStateFlags.gameModeEnabled;

            if (engineStateFlags.worldEditorModeEnabled == false) {
                glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            else {
                glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            IOEvents::updateGameState(engineStateFlags);
        }

        IOEvents::updateKeyboard(key, 0, 0);
    }
    else if (action == GLFW_RELEASE) {
        IOEvents::releaseKeyboard(key, 0, 0);
    }
    else if (action == GLFW_REPEAT) {
        EngineStateFlags engineStateFlags = EngineState::getEngineState();
        if (engineStateFlags.worldEditorModeEnabled) {
            IOEvents::updateKeyboard(key, 0, 0);
        }
    }
}

//One frame draw update call
void IOEventDistributor::_drawUpdate() {

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL)
    {
        while (!_quit) {
            if (_timeEvents.size()) {
                uint64_t now = nowMs();
                TimeEvent event = _timeEvents.top();
                if (event.time < now) {
                    _timeEvents.pop();
                    event.pfnCallback();
                }
            }

            //Only draw when a framerate trigger event has been received from the master clock
            if (_renderNow > 0) {
                _renderLock.lock();
                do {
                    IOEvents::updateDraw(_window);
                    //Decrement trigger
                    _renderNow--;
                } while (_renderNow > 0);
                _renderLock.unlock();
            }

            //IOEvents::updateDraw(_window);
        }
    }
    else {

        // this struct holds Windows event messages
        MSG msg = { 0 };
        // main loop
        while (true) {
            // check to see if any messages are waiting in the queue
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                // translate keystroke messages into the right format
                TranslateMessage(&msg);
                DispatchMessage(&msg);

                // check to see if it's time to quit
                if (msg.message == WM_QUIT)
                    break;
            }
            else {
                //prev x and y position ensure that the valid mouse movement is complete
                //prior to set the cursor position back to the middle of the screen
                if (_prevMouseX == 0 &&
                    _prevMouseX == 0) {
                    SetCursorPos(IOEventDistributor::screenPixelWidth  / 2,
                                 IOEventDistributor::screenPixelHeight / 2);

                    _prevMouseX = IOEventDistributor::screenPixelWidth  / 2;
                    _prevMouseY = IOEventDistributor::screenPixelHeight / 2;
                }
            }


            IOEvents::updateDraw(_window);
        }
    }

}

//All mouse input will be notified here
void IOEventDistributor::_mouseUpdate(GLFWwindow* window,
                                      double      x,
                                      double      y) {

    IOEvents::updateMouse(x, y);
}

void IOEventDistributor::_mouseClick(GLFWwindow* window,
                                     int         button,
                                     int         action,
                                     int         mods) {
    double xpos, ypos;
    //getting cursor position
    glfwGetCursorPos(_window,
                     &xpos,
                     &ypos);

    IOEvents::updateMouseClick(button,
                               action,
                               static_cast<int>(xpos),
                               static_cast<int>(ypos));
}

void IOEventDistributor::_frameRateTrigger(int milliSeconds) {
    //Triggers the simple context to draw a frame
    _renderLock.lock();
    _renderNow++;
    _renderLock.unlock();
}
