#pragma once
#include "Vector4.h"
#include <math.h>

const float PI = 3.14159265f;
const float PI_OVER_180 = PI / 180.0f;

class Matrix {


    //OPENGL Matrix layout
    //P is a dimensional position vector in x,y,z
    //X coordinate vector
    //Y coordinate vector
    //Z coordinate vector
    //| Xx Xy Xz Px |  
    //| Yx Yy Yz Py |  
    //| Zx Zy Zz Pz |  
    //| 0  0  0  1  |  

    //To Create a Translation Matrix Change P
    //To Create a Rotation Matrix Change X,Y and Z
    //To Create a Scaling Matrix Change Xx, Yy, Zz

    //Translation Matrix of a +5 change in X position
    //| 1 0 0 5 |  
    //| 0 1 0 0 |  
    //| 0 0 1 0 |  
    //| 0 0 0 1 |  

    //Rotation Matrix of a theta change around X axis
    //| 1  0    0   0 |  
    //| 0 cos -sin  0 |  
    //| 0 sin  cos  0 |  
    //| 0  0    0   1 |  

    //Rotation Matrix of a theta change around Y axis
    //| cos 0  sin  0 |  
    //|  0  1   0   0 |  
    //|-sin 0  cos  0 |  
    //|  0  0   0   1 |  

    //Rotation Matrix of a theta change around Z axis
    //| cos -sin 0  0 |  
    //| sin  cos 0  0 |  
    //|  0    0  1  0 |  
    //|  0    0  0  1 |  

    //Scale Matrix of 2
    //| 2 0 0 0 |  
    //| 0 2 0 0 |  
    //| 0 0 2 0 |  
    //| 0 0 0 1 |  

    
    float _matrix[16];
    int _rows;
    int _columns;

public:
    Matrix();
    Matrix(float *mat);

	//ALWAYS CALL THIS FUNCTION BEFORE SENDING A MATRIX TO A SHADER!!!!!!!!!!!!!
	Matrix getGLFormat(); //Returns the matrix in column major format which is how GL works

    Matrix transpose(); //Returns transpose of matrix
	Matrix operator * (Matrix mat); 
    Vector4 operator * (Vector4 vec);
    float* getFlatBuffer();
    void display();

    static Matrix rotationAroundX(float degrees); //Build rotation matrix around the x axis
    static Matrix rotationAroundY(float degrees); //Build rotation matrix around the y axis
    static Matrix rotationAroundZ(float degrees); //Build rotation matrix around the z axis
    static Matrix translation(float x, float y, float z); //Build translation matrix
    static Matrix scale(float scalar); //Build a scale matrix

    //Used specific for view transformations associated with a camera view
    static Matrix cameraRotationAroundX(float degrees); //Build rotation matrix around the x axis
    static Matrix cameraRotationAroundY(float degrees); //Build rotation matrix around the y axis
    static Matrix cameraRotationAroundZ(float degrees); //Build rotation matrix around the z axis
    static Matrix cameraTranslation(float x, float y, float z); //Build translation matrix
	static Matrix cameraProjection(float angleOfView, float aspectRatio, float near, float far);
}; 