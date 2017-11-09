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
    //translate.display();
    Matrix rotateY = Matrix::rotationAroundY(90.0); //middle transformation 90 degrees around y axis
    //rotateY.display();
    Matrix translateAgain = Matrix::translation(0.0, 0.0, -2.0); //first transformation in -2 z axis
    //translateAgain.display();

    Matrix finalTransformation = translate * rotateY * translateAgain;
    Vector4 pos = translate * rotateY * translateAgain * Vector4(0, 0, 0, 1); //Origin vector
    //pos.display();

    //Multiply matrices as we go and store in one
    Matrix transformation;
    transformation = transformation * Matrix::translation(0.0, 0.0, -10.0);
    transformation = transformation * Matrix::rotationAroundY(90.0);
    transformation = transformation * Matrix::translation(0.0, 0.0, -2.0);
    Vector4 pos2 = transformation * Vector4(0, 0, 0, 1);
    //pos2.display();

    //Multiple ways to get results
    Vector4 pos3 = Matrix::translation(0.0, 0.0, -10.0) * Matrix::rotationAroundY(90.0) * Matrix::translation(0.0, 0.0, -2.0) * Vector4(0, 0, 0, 1);
    //pos3.display();

    //Send the width and height in pixel units and the near and far plane to describe the view frustum
    SceneManager sceneManager(&argc, argv, 1920, 1080, 0.1, 1000); //Manages the camera view and models in scene

    std::cout << "Done matrix testing!" << std::endl;

    return 0;
}