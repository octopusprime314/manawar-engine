#include "SimpleContext.h"
#include "ViewManagerEvents.h"
#include "MasterClock.h"

int         SimpleContext::_renderNow = 0;
std::mutex  SimpleContext::_renderLock;   
GLFWwindow* SimpleContext::_window;
bool        SimpleContext::_quit = false;

SimpleContext::SimpleContext(int* argc, char** argv, unsigned int viewportWidth, unsigned int viewportHeight) {

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
        char buffer[1024];
        snprintf(buffer, sizeof(buffer), "GLFW [0x%Xu] %s\n", code, pMsg);
        std::cerr << buffer << std::endl;
    });

    //Create a glfw window for a context
    _window = glfwCreateWindow(viewportWidth, viewportHeight, "ReBoot", NULL, NULL);
    if (!_window) {
        // Window or OpenGL context creation failed
        // The error callback above will tell us what happened.
        std::abort();
    }
    glfwMakeContextCurrent(_window); //Make current opengl context current

    //Callbacks
    glfwSetKeyCallback(_window, &SimpleContext::_keyboardUpdate);
    glfwSetCursorPosCallback(_window, &SimpleContext::_mouseUpdate);

    //Sets atleast one extra render buffer for double buffering to prevent screen tearing
    //glfwSwapInterval(1); //Enables 60 hz vsync
    glfwSwapInterval(0); //Disables 60 hz vsync

    if (gl3wInit()) {
        std::cout << "failed to initialize OpenGL\n" << std::endl;
    }
    if (!gl3wIsSupported(3, 0)) {
        std::cout << "OpenGL 3.2 not supported\n" << std::endl;
    }
    printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION),
        glGetString(GL_SHADING_LANGUAGE_VERSION));

    //PER SAMPLE PROCESSING DEFAULTS
    glClearDepth(1.0); //Enables Clearing Of The Depth Buffer
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST); //Ensure depth test happens
    glDepthFunc(GL_LESS); //Keep the fragment closest
    

    //Disable mouse cursor view
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    MasterClock* masterClock = MasterClock::instance();
    masterClock->setFrameRate(60); //Establishes the frame rate of the draw context
    masterClock->subscribeFrameRate(std::bind(_frameRateTrigger, std::placeholders::_1));

}

void SimpleContext::run() {
    _drawUpdate();
}

void SimpleContext::subscribeToKeyboard(std::function<void(int, int, int)> func) { //Use this call to connect functions to key updates
    _events.subscribeToKeyboard(func);
}
void SimpleContext::subscribeToReleaseKeyboard(std::function<void(int, int, int)> func) { //Use this call to connect functions to key updates
    _events.subscribeToKeyboard(func);
}
void SimpleContext::subscribeToMouse(std::function<void(double, double)> func) { //Use this call to connect functions to mouse updates
    _events.subscribeToMouse(func);
}
void SimpleContext::subscribeToDraw(std::function<void()> func) { //Use this call to connect functions to draw updates
    _events.subscribeToDraw(func);
}

//All keyboard input from glfw will be notified here
void SimpleContext::_keyboardUpdate(GLFWwindow* window, int key, int scancode, int action, int mods) {

    if(action == GLFW_PRESS){

        if (key == GLFW_KEY_ESCAPE) { //Escape key pressed, hard exit no cleanup, TODO FIX THIS!!!!
            glfwDestroyWindow(_window);
            glfwTerminate();
            _quit = true;
            exit(0);
        }

        SimpleContextEvents::updateKeyboard(key, 0, 0);
    }
    else if(action == GLFW_RELEASE){
        SimpleContextEvents::releaseKeyboard(key, 0, 0);
    }
    
}

//One frame draw update call
void SimpleContext::_drawUpdate() {
    
    while (!_quit) {

        //Only draw when a framerate trigger event has been received from the master clock
        //if(_renderNow > 0){
            //_renderLock.lock();

            //do {
                SimpleContextEvents::updateDraw(_window);
                //Decrement trigger
                //_renderNow--;
            //} while(_renderNow > 0);

            //_renderLock.unlock();
        //}
    }
}

//All mouse input will be notified here
void SimpleContext::_mouseUpdate(GLFWwindow* window, double x, double y) {

    SimpleContextEvents::updateMouse(x, y);

    //Bring cursor back to center position
    glfwSetCursorPos(_window, screenPixelWidth/2, screenPixelHeight/2); 
}

void SimpleContext::_frameRateTrigger(int milliSeconds){
    //Triggers the simple context to draw a frame
    _renderLock.lock();
    _renderNow++;
    _renderLock.unlock();
}