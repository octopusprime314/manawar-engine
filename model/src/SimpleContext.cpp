#include "SimpleContext.h"
#include "ViewManagerEvents.h"
#include "MasterClock.h"

int         SimpleContext::_renderNow = 0;
std::mutex  SimpleContext::_renderLock;   

SimpleContext::SimpleContext(int* argc, char** argv, unsigned int viewportWidth, unsigned int viewportHeight) {

    glutInit(argc, argv); //initialize glut

    /* Select type of Display mode:
    Double buffer
    RGBA color
    Alpha components supported
    Depth buffer */
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_ACCUM | GLUT_DEPTH);

    //WINDOW CONTEXT SETTINGS
    glutInitWindowPosition(0, 0); //Position it at the top
    glutInitWindowSize(viewportWidth, viewportHeight); //viewport pixel width and height
    glutCreateWindow("ReBoot!"); //create a window called ReBoot

    //GLUT FUNCTION CALLBACKS
    glutKeyboardFunc(&SimpleContext::_keyboardUpdate); //Set function callback for keyboard input
	glutKeyboardUpFunc(&SimpleContext::_keyboardRelease); //Set function callback for key release event
    glutDisplayFunc(_drawUpdate); //Set function callback for draw updates
    glutIdleFunc(_drawUpdate); //Set function callback for draw updates when no events are occuring
    glutMouseFunc(_mouseUpdate); //Set function callback for mouse press input
    glutMotionFunc(_mouseUpdate); //Set function callback for movement while pressing mouse 
    glutPassiveMotionFunc(_mouseUpdate); //Set function callback for mouse movement without press
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF); //Disable key repeat events, only want to keep track of original press and release
    glutIgnoreKeyRepeat(1); //ignore repeats

    glewInit(); //initialize the extension gl call functionality

    //PER SAMPLE PROCESSING DEFAULTS
    glEnable(GL_TEXTURE_2D); //Enable use of textures
    glutSetCursor(GLUT_CURSOR_NONE); //Disable cursor icon
    glClearDepth(1.0); //Enables Clearing Of The Depth Buffer
    glShadeModel(GL_SMOOTH); //Shade models smoothly
    glEnable(GL_DEPTH_TEST); //Ensure depth test happens
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); //Ask for nicest perspective correction
    glDepthFunc(GL_LESS); //Keep the fragment closest
    glEnable(GL_LIGHTING); //Lighting enabled

    MasterClock* masterClock = MasterClock::instance();
    masterClock->setFrameRate(60); //Establishes the frame rate of the draw context
    masterClock->subscribeFrameRate(std::bind(_frameRateTrigger, std::placeholders::_1));

}

void SimpleContext::run() {

    glutMainLoop();
}

void SimpleContext::subscribeToKeyboard(std::function<void(unsigned char, int, int)> func) { //Use this call to connect functions to key updates
    _events.subscribeToKeyboard(func);
}
void SimpleContext::subscribeToReleaseKeyboard(std::function<void(unsigned char, int, int)> func) { //Use this call to connect functions to key updates
    _events.subscribeToKeyboard(func);
}
void SimpleContext::subscribeToMouse(std::function<void(int, int, int, int)> func) { //Use this call to connect functions to mouse updates
    _events.subscribeToMouse(func);
}
void SimpleContext::subscribeToDraw(std::function<void()> func) { //Use this call to connect functions to draw updates
    _events.subscribeToDraw(func);
}

//All keyboard input from glut will be notified here
void SimpleContext::_keyboardUpdate(unsigned char key, int x, int y) {

    SimpleContextEvents::updateKeyboard(key, x, y);
}

void SimpleContext::_keyboardRelease(unsigned char key, int x, int y){
	SimpleContextEvents::releaseKeyboard(key, x, y);
}

//One frame draw update call
void SimpleContext::_drawUpdate() {

    ////Only draw when a framerate trigger event has been received from the master clock
    //if(_renderNow > 0){
    //    _renderLock.lock();

    //    do {
    SimpleContextEvents::updateDraw();
    //        //Decrement trigger
    //        _renderNow--;
    //    } while(_renderNow > 0);

    //    _renderLock.unlock();
    //}
}

//All mouse input presses from glut will be notified here
void SimpleContext::_mouseUpdate(int button, int state, int x, int y) {

    SimpleContextEvents::updateMouse(button, state, x, y);
}

//All passive mouse movement input from glut will be notified here
void SimpleContext::_mouseUpdate(int x, int y) {

    SimpleContextEvents::updateMouse(x, y);
}

void SimpleContext::_frameRateTrigger(int milliSeconds){
    //Triggers the simple context to draw a frame
    _renderLock.lock();
    _renderNow++;
    _renderLock.unlock();
}