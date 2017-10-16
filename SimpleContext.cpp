#include "SimpleContext.h"

std::vector<std::function<void(unsigned char, int, int)>> SimpleContext::_keyboardFuncs;
std::vector<std::function<void(int, int, int, int)>> SimpleContext::_mouseFuncs;

SimpleContext::SimpleContext(int* argc, char** argv) {
		
	glutInit(argc, argv); //initialize glut
	
	/* Select type of Display mode:   
	Double buffer 
	RGBA color
	Alpha components supported 
	Depth buffer */  
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_ACCUM | GLUT_DEPTH); 

	//WINDOW CONTEXT SETTINGS
	glutInitWindowPosition(0,0); //Position it at the top
	glutInitWindowSize(1080, 1920); //make it 1080x1920
	glutCreateWindow("ReBoot!"); //create a window called ReBoot
		
	//GLUT FUNCTION CALLBACKS
	glutKeyboardFunc(_keyboardUpdate); //Set function callback for keyboard input
	glutDisplayFunc(_drawUpdate); //Set function callback for draw updates
	glutIdleFunc(_drawUpdate); //Set function callback for draw updates when no events are occuring
	glutMouseFunc(_mouseUpdate); //Set function callback for mouse press input
	glutMotionFunc(_mouseUpdate); //Set function callback for movement while pressing mouse 
	glutPassiveMotionFunc(_mouseUpdate); //Set function callback for mouse movement without press

	glewInit(); //initialize the extension gl call functionality

	//PER SAMPLE PROCESSING DEFAULTS
	glEnable(GL_TEXTURE_2D);
	glutSetCursor(GLUT_CURSOR_NONE);
	glClearDepth(1.0);						// Enables Clearing Of The Depth Buffer
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Ask for nicest perspective correction
	glDepthFunc(GL_LESS);
	glEnable(GL_LIGHTING);

}

void SimpleContext::run(){
	glutMainLoop();
}

void SimpleContext::subscribeToKeyboard(std::function<void(unsigned char, int, int)> func){ //Use this call to connect functions to key updates
	_keyboardFuncs.push_back(func);
}
void SimpleContext::subscribeToMouse(std::function<void(int, int, int, int)> func){ //Use this call to connect functions to mouse updates
	_mouseFuncs.push_back(func);
}

//All keyboard input from glut will be notified here
void SimpleContext::_keyboardUpdate(unsigned char key, int x, int y){
	for(auto func : _keyboardFuncs){
		func(key,x,y); //Call keyboard update
	}
}

//One frame draw update call
void SimpleContext::_drawUpdate(){

	glutSwapBuffers(); // Double buffering
}

//All mouse input presses from glut will be notified here
void SimpleContext::_mouseUpdate(int button, int state, int x, int y){
	for(auto func : _mouseFuncs){
		func(button,state,x,y); //Call mouse click update
	}
}

//All passive mouse movement input from glut will be notified here
void SimpleContext::_mouseUpdate(int x, int y){
	for(auto func : _mouseFuncs){
		func(0,0,x,y); //Call mouse movement update 
	}
}

