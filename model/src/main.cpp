#include "Matrix.h"
#include "Vector4.h"
#include "SimpleContext.h"
#include <iostream>
#include <thread>
#include "SceneManager.h"

int main(int argc, char** argv) {

    //Send the width and height in pixel units and the near and far plane to describe the view frustum
    SceneManager sceneManager(&argc, argv, 1920, 1080, 0.1, 1000); //Manages the camera view and models in scene

    return 0;
}