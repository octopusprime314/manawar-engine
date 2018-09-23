#include "IOEventDistributor.h"
#include "ViewEvents.h"
#include "MasterClock.h"

int         IOEventDistributor::_renderNow = 0;
std::mutex  IOEventDistributor::_renderLock;
GLFWwindow* IOEventDistributor::_window;
bool        IOEventDistributor::_quit = false;
int         IOEventDistributor::screenPixelWidth = 1920;
int         IOEventDistributor::screenPixelHeight = 1080;

std::priority_queue<TimeEvent> IOEventDistributor::_timeEvents; // Events that trigger at a specific time

uint64_t nowMs() {
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;

    return duration_cast<milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

IOEventDistributor::IOEventDistributor(int* argc, char** argv) {

    char workingDir[1024] = "";
    GetCurrentDirectory(sizeof(workingDir), workingDir);
    std::cout << "Working directory: " << workingDir << std::endl;

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

    int monitorResoltuionX = mode->width;
    int monitorResoltuionY = mode->height;

    //WINDOWED MODE
    _window = glfwCreateWindow(monitorResoltuionX, monitorResoltuionY, "ReBoot", NULL, NULL);
    //FULLSCREEN MODE
    //_window = glfwCreateWindow(viewportWidth, viewportHeight, "ReBoot", glfwGetPrimaryMonitor(), NULL);
    if (!_window) {
        // Window or OpenGL context creation failed
        // The error callback above will tell us what happened.
        std::abort();
    }
    glfwMakeContextCurrent(_window); //Make current opengl context current

    int width, height;
    glfwGetWindowSize(_window, &width, &height);

    IOEventDistributor::screenPixelWidth = width;
    IOEventDistributor::screenPixelHeight = height;

    //Callbacks
    glfwSetKeyCallback(_window, &IOEventDistributor::_keyboardUpdate);
    glfwSetCursorPosCallback(_window, &IOEventDistributor::_mouseUpdate);
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
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //Color buffer settings
    glClearColor(0.0, 0.0, 0.0, 0.0);
    //Stencil buffer settings
    glClearStencil(0);

    //Disable mouse cursor view
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    MasterClock* masterClock = MasterClock::instance();
    masterClock->setFrameRate(60); //Establishes the frame rate of the draw context
    masterClock->subscribeFrameRate(std::bind(_frameRateTrigger, std::placeholders::_1));

    // Hard coded debug events
    TimeEvent::Callback* debugCallback = []() { printf("%g\n", nowMs() / 1e3f); };

    /*uint64_t now = nowMs();
    constexpr uint64_t sec = 1000;
    constexpr uint64_t DEMO_LENGTH = (60 + 17)*sec;
    _timeEvents.push({ now + DEMO_LENGTH, []() {
        ::std::exit(0);
    }});*/
}

void IOEventDistributor::run() {
    _drawUpdate();
}

void IOEventDistributor::subscribeToKeyboard(std::function<void(int, int, int)> func) { //Use this call to connect functions to key updates
    _events.subscribeToKeyboard(func);
}
void IOEventDistributor::subscribeToReleaseKeyboard(std::function<void(int, int, int)> func) { //Use this call to connect functions to key updates
    _events.subscribeToKeyboard(func);
}
void IOEventDistributor::subscribeToMouse(std::function<void(double, double)> func) { //Use this call to connect functions to mouse updates
    _events.subscribeToMouse(func);
}
void IOEventDistributor::subscribeToDraw(std::function<void()> func) { //Use this call to connect functions to draw updates
    _events.subscribeToDraw(func);
}
void IOEventDistributor::subscribeToGameState(std::function<void(EngineStateFlags)> func) {
    _events.subscribeToGameState(func);
}
void IOEventDistributor::updateGameState(EngineStateFlags state) {
    IOEvents::updateGameState(state);
}

//All keyboard input from glfw will be notified here
void IOEventDistributor::_keyboardUpdate(GLFWwindow* window, int key, int scancode, int action, int mods) {

    if (action == GLFW_PRESS) {

        if (key == GLFW_KEY_ESCAPE) { //Escape key pressed, hard exit no cleanup, TODO FIX THIS!!!!
            glfwDestroyWindow(_window);
            glfwTerminate();
            _quit = true;
            exit(0);
        }

        if (key == GLFW_KEY_GRAVE_ACCENT) { //toggle between world editor and game mode
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
        //if(_renderNow > 0){
            //_renderLock.lock();

            //do {
        IOEvents::updateDraw(_window);
        //Decrement trigger
        //_renderNow--;
    //} while(_renderNow > 0);

    //_renderLock.unlock();
//}
    }
}

//All mouse input will be notified here
void IOEventDistributor::_mouseUpdate(GLFWwindow* window, double x, double y) {

    IOEvents::updateMouse(x, y);
}

void IOEventDistributor::_mouseClick(GLFWwindow* window, int button, int action, int mods) {
    double xpos, ypos;
    //getting cursor position
    glfwGetCursorPos(_window, &xpos, &ypos);
    IOEvents::updateMouseClick(button, action, static_cast<int>(xpos), static_cast<int>(ypos));
}


void IOEventDistributor::_frameRateTrigger(int milliSeconds) {
    //Triggers the simple context to draw a frame
    _renderLock.lock();
    _renderNow++;
    _renderLock.unlock();
}
