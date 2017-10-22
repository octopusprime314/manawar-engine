#include "Matrix.h"
#include "Vector4.h"
#include "SimpleContext.h"
#include <iostream>
#include <thread>
#include "SceneManager.h"

int main(int argc, char** argv) {

    Matrix matTester;

    //Build matrices up first then multiply
    Matrix translate = Matrix::translation(0.0, 0.0, -10.0); //last transformation in -10 z axis
    translate.display();
    Matrix rotateY = Matrix::rotationAroundY(90.0); //middle transformation 90 degrees around y axis
    rotateY.display();
    Matrix translateAgain = Matrix::translation(0.0, 0.0, -2.0); //first transformation in -2 z axis
    translateAgain.display();

    Matrix finalTransformation = translate * rotateY * translateAgain;
    Vector4 pos = translate * rotateY * translateAgain * Vector4(0,0,0,1); //Origin vector
    pos.display();

    //Multiply matrices as we go and store in one
    Matrix transformation;
    transformation = transformation * Matrix::translation(0.0, 0.0, -10.0);
    transformation = transformation * Matrix::rotationAroundY(90.0);
    transformation = transformation * Matrix::translation(0.0, 0.0, -2.0);
    Vector4 pos2 = transformation * Vector4(0,0,0,1);
    pos2.display();

    //Multiple ways to get results
    Vector4 pos3 = Matrix::translation(0.0, 0.0, -10.0) * Matrix::rotationAroundY(90.0) * Matrix::translation(0.0, 0.0, -2.0) * Vector4(0, 0, 0, 1);
    pos3.display();

	//Create instance of glut wrapper class context
	//GLUT context can only run on main thread!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//PLEASE DO NOT THREAD GLUT CALLS
	SimpleContext con(&argc, argv);

	SceneManager sceneManager; //Manages the camera view and models in scene

	//Start rendering!
	con.run();

    std::cout << "Done matrix testing!" << std::endl;

	
    ////ViewManager acts as camera moving through space
    //ViewManager camera;
	
    ////View transformations always apply opposite transformations because objects move around camera
    ////If the user wants to move foward in z 10 meters then the view matrix must be translated in z by -10
    ////If the user wants to rotate around the y axis by -90 then the rotation appliec must be + 90 around the y axis
    //camera.applyTransform(Matrix::cameraTranslation(0.0, 0.0, 10.0)); //Camera Translation in x applies opposite translation
    //camera.applyTransform(Matrix::cameraRotationAroundY(90.0)); //apply rotation based on user rotation

	return 0;
}