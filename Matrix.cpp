#include "Matrix.h"
#include <iostream>
#include <iomanip>
using namespace std;

Matrix::Matrix() {
	 //Identity 4x4 homogenous matrix
	_matrix[0]  = 1.0,  _matrix[1] = 0.0,  _matrix[2] = 0.0,  _matrix[3] = 0.0;
    _matrix[4]  = 0.0,  _matrix[5] = 1.0,  _matrix[6] = 0.0,  _matrix[7] = 0.0;
    _matrix[8]  = 0.0,  _matrix[9] = 0.0, _matrix[10] = 1.0, _matrix[11] = 0.0;
    _matrix[12] = 0.0, _matrix[13] = 0.0, _matrix[14] = 0.0, _matrix[15] = 1.0;
}

Matrix::Matrix(float* mat) {

    _matrix[0] = mat[0],   _matrix[1] = mat[1],   _matrix[2] = mat[2],   _matrix[3] = mat[3];
    _matrix[4] = mat[4],   _matrix[5] = mat[5],   _matrix[6] = mat[6],   _matrix[7] = mat[7];
    _matrix[8] = mat[8],   _matrix[9] = mat[9],   _matrix[10] = mat[10], _matrix[11] = mat[11];
    _matrix[12] = mat[12], _matrix[13] = mat[13], _matrix[14] = mat[14], _matrix[15] = mat[15];
}

float* Matrix::getFlatBuffer() {
    return _matrix;
}

Matrix Matrix::transpose() {
    Matrix matrix(_matrix);
    float* mat = matrix.getFlatBuffer();

	mat[0] = _matrix[0], mat[1] = _matrix[4], mat[2] = _matrix[8], mat[3] = _matrix[12];
    mat[4] = _matrix[1], mat[5] = _matrix[5], mat[6] = _matrix[9], mat[7] = _matrix[13];
    mat[8] = _matrix[2], mat[9] = _matrix[6], mat[10] = _matrix[10], mat[11] = _matrix[14];
    mat[12] = _matrix[3], mat[13] = _matrix[7], mat[14] = _matrix[11], mat[15] = _matrix[15];

	return matrix;
}

Vector4 Matrix::operator * (Vector4 vec) {
    Vector4 result; //Get underlying vector memory
    float* vector = vec.getFlatBuffer();

    float x = _matrix[0]  * vector[0] + _matrix[1]  * vector[1] + _matrix[2]  * vector[2] + _matrix[3]  * vector[3];
    float y = _matrix[4]  * vector[0] + _matrix[5]  * vector[1] + _matrix[6]  * vector[2] + _matrix[7]  * vector[3];
    float z = _matrix[8]  * vector[0] + _matrix[9]  * vector[1] + _matrix[10] * vector[2] + _matrix[11] * vector[3];
    float w = _matrix[12] * vector[0] + _matrix[13] * vector[1] + _matrix[14] * vector[2] + _matrix[15] * vector[3];

    return Vector4(x,y,z,w);
}

Matrix Matrix::operator * (Matrix mat) {

    float result[16]; //Get underlying matrix memory
    float* matBuff = mat.getFlatBuffer();

    result[0]  = _matrix[0] * matBuff[0] + _matrix[1] * matBuff[4] + _matrix[2] * matBuff[8]  + _matrix[3] * matBuff[12];
    result[1]  = _matrix[0] * matBuff[1] + _matrix[1] * matBuff[5] + _matrix[2] * matBuff[9]  + _matrix[3] * matBuff[13];
    result[2]  = _matrix[0] * matBuff[2] + _matrix[1] * matBuff[6] + _matrix[2] * matBuff[10] + _matrix[3] * matBuff[14];
    result[3]  = _matrix[0] * matBuff[3] + _matrix[1] * matBuff[7] + _matrix[2] * matBuff[11] + _matrix[3] * matBuff[15];

    result[4]  = _matrix[4] * matBuff[0] + _matrix[5] * matBuff[4] + _matrix[6] * matBuff[8]  + _matrix[7] * matBuff[12];
    result[5]  = _matrix[4] * matBuff[1] + _matrix[5] * matBuff[5] + _matrix[6] * matBuff[9]  + _matrix[7] * matBuff[13];
    result[6]  = _matrix[4] * matBuff[2] + _matrix[5] * matBuff[6] + _matrix[6] * matBuff[10] + _matrix[7] * matBuff[14];
    result[7]  = _matrix[4] * matBuff[3] + _matrix[5] * matBuff[7] + _matrix[6] * matBuff[11] + _matrix[7] * matBuff[15];

    result[8]  = _matrix[8] * matBuff[0] + _matrix[9] * matBuff[4] + _matrix[10] * matBuff[8]  + _matrix[11] * matBuff[12];
    result[9]  = _matrix[8] * matBuff[1] + _matrix[9] * matBuff[5] + _matrix[10] * matBuff[9]  + _matrix[11] * matBuff[13];
    result[10] = _matrix[8] * matBuff[2] + _matrix[9] * matBuff[6] + _matrix[10] * matBuff[10] + _matrix[11] * matBuff[14];
    result[11] = _matrix[8] * matBuff[3] + _matrix[9] * matBuff[7] + _matrix[10] * matBuff[11] + _matrix[11] * matBuff[15];

    result[12] = _matrix[12] * matBuff[0] + _matrix[13] * matBuff[4] + _matrix[14] * matBuff[8]  + _matrix[15] * matBuff[12];
    result[13] = _matrix[12] * matBuff[1] + _matrix[13] * matBuff[5] + _matrix[14] * matBuff[9]  + _matrix[15] * matBuff[13];
    result[14] = _matrix[12] * matBuff[2] + _matrix[13] * matBuff[6] + _matrix[14] * matBuff[10] + _matrix[15] * matBuff[14];
    result[15] = _matrix[12] * matBuff[3] + _matrix[13] * matBuff[7] + _matrix[14] * matBuff[11] + _matrix[15] * matBuff[15];

    return Matrix(result);
}
//Rotation Matrix of a theta change around X axis
//| 1  0    0   0 |  
//| 0 cos -sin  0 |  
//| 0 sin  cos  0 |  
//| 0  0    0   1 |  
Matrix Matrix::rotationAroundX(float degrees) {
    float result[16]; //Get underlying matrix memory
    float theta = degrees * PI_OVER_180;

    result[0]  = 1.0, result[1]  =    0.0,     result[2]  =    0.0,      result[3]  = 0.0;
    result[4]  = 0.0, result[5]  = cos(theta), result[6]  = -sin(theta), result[7]  = 0.0;
    result[8]  = 0.0, result[9]  = sin(theta), result[10] =  cos(theta), result[11] = 0.0;
    result[12] = 0.0, result[13] =    0.0,     result[14] =    0.0,      result[15] = 1.0;

    return Matrix(result);
}


//Rotation Matrix of a theta change around Y axis
//| cos 0  sin  0 |  
//|  0  1   0   0 |  
//|-sin 0  cos  0 |  
//|  0  0   0   1 |  
Matrix Matrix::rotationAroundY(float degrees) {
    float result[16]; //Get underlying matrix memory
    float theta = degrees * PI_OVER_180;

    result[0]  = cos(theta),  result[1]  = 0.0, result[2]  = sin(theta), result[3]  = 0.0;
    result[4]  =    0.0,      result[5]  = 1.0, result[6]  =    0.0,     result[7]  = 0.0;
    result[8]  = -sin(theta), result[9]  = 0.0, result[10] = cos(theta), result[11] = 0.0;
    result[12] =   0.0,       result[13] = 0.0, result[14] =    0.0,     result[15] = 1.0;

    return Matrix(result);
}


//Rotation Matrix of a theta change around Z axis
//| cos -sin 0  0 |  
//| sin  cos 0  0 |  
//|  0    0  1  0 |  
//|  0    0  0  1 |  
Matrix Matrix::rotationAroundZ(float degrees) {
    float result[16]; //Get underlying matrix memory
    float theta = degrees * PI_OVER_180;

    result[0]  = cos(theta), result[1]  = -sin(theta), result[2]  = 0.0, result[3]  = 0.0;
    result[4]  = sin(theta), result[5]  = cos(theta),  result[6]  = 0.0, result[7]  = 0.0;
    result[8]  =    0.0,     result[9]  =     0.0,     result[10] = 1.0, result[11] = 0.0;
    result[12] =    0.0,     result[13] =     0.0,     result[14] = 0.0, result[15] = 1.0;

    return Matrix(result);
}

//Translation Matrix of a +5 change in X position
//| 1 0 0 5 |  
//| 0 1 0 0 |  
//| 0 0 1 0 |  
//| 0 0 0 1 |  
Matrix Matrix::translation(float x, float y, float z) {
    float result[16]; //Get underlying matrix memory

    result[0]  = 1.0, result[1]  = 0.0, result[2]  = 0.0, result[3]  = x;
    result[4]  = 0.0, result[5]  = 1.0, result[6]  = 0.0, result[7]  = y;
    result[8]  = 0.0, result[9]  = 0.0, result[10] = 1.0, result[11] = z;
    result[12] = 0.0, result[13] = 0.0, result[14] = 0.0, result[15] = 1.0;

    return Matrix(result);
}

//Scale Matrix of 2
//| 2 0 0 0 |  
//| 0 2 0 0 |  
//| 0 0 2 0 |  
//| 0 0 0 1 |  
Matrix Matrix::scale(float scalar) {
    float result[16]; //Get underlying matrix memory

    result[0]  = scalar, result[1]  = 0.0,    result[2]  = 0.0,    result[3]  = 0.0;
    result[4]  = 0.0,    result[5]  = scalar, result[6]  = 0.0,    result[7]  = 0.0;
    result[8]  = 0.0,    result[9]  = 0.0,    result[10] = scalar, result[11] = 0.0;
    result[12] = 0.0,    result[13] = 0.0,    result[14] = 0.0,    result[15] = 1.0;

    return Matrix(result);
}

//Camera Rotation Matrix is opposite
Matrix Matrix::cameraRotationAroundX(float degrees) {
    float result[16]; //Get underlying matrix memory
    float theta = -degrees * PI_OVER_180;

    result[0] = 1.0, result[1] = 0.0, result[2] = 0.0, result[3] = 0.0;
    result[4] = 0.0, result[5] = cos(theta), result[6] = -sin(theta), result[7] = 0.0;
    result[8] = 0.0, result[9] = sin(theta), result[10] = cos(theta), result[11] = 0.0;
    result[12] = 0.0, result[13] = 0.0, result[14] = 0.0, result[15] = 1.0;

    return Matrix(result);
}


//Camera Rotation Matrix is opposite
Matrix Matrix::cameraRotationAroundY(float degrees) {
    float result[16]; //Get underlying matrix memory
    float theta = -degrees * PI_OVER_180;

    result[0] = cos(theta), result[1] = 0.0, result[2] = sin(theta), result[3] = 0.0;
    result[4] = 0.0, result[5] = 1.0, result[6] = 0.0, result[7] = 0.0;
    result[8] = -sin(theta), result[9] = 0.0, result[10] = cos(theta), result[11] = 0.0;
    result[12] = 0.0, result[13] = 0.0, result[14] = 0.0, result[15] = 1.0;

    return Matrix(result);
}

//Camera Rotation Matrix is opposite
Matrix Matrix::cameraRotationAroundZ(float degrees) {
    float result[16]; //Get underlying matrix memory
    float theta = -degrees * PI_OVER_180;

    result[0] = cos(theta), result[1] = -sin(theta), result[2] = 0.0, result[3] = 0.0;
    result[4] = sin(theta), result[5] = cos(theta), result[6] = 0.0, result[7] = 0.0;
    result[8] = 0.0, result[9] = 0.0, result[10] = 1.0, result[11] = 0.0;
    result[12] = 0.0, result[13] = 0.0, result[14] = 0.0, result[15] = 1.0;

    return Matrix(result);
}

//Camera Translation Matrix is opposite
Matrix Matrix::cameraTranslation(float x, float y, float z) {
    float result[16]; //Get underlying matrix memory

    result[0] = 1.0, result[1] = 0.0, result[2] = 0.0, result[3] = -x;
    result[4] = 0.0, result[5] = 1.0, result[6] = 0.0, result[7] = -y;
    result[8] = 0.0, result[9] = 0.0, result[10] = 1.0, result[11] = -z;
    result[12] = 0.0, result[13] = 0.0, result[14] = 0.0, result[15] = 1.0;

    return Matrix(result);
}

Matrix Matrix::cameraProjection(float angleOfView, float imageAspectRatio, float n, float f){
   
	float result[16];

	//Setup components of projection
	float scale = static_cast<float>(tan(angleOfView * 0.5 * PI_OVER_180)) * n; //scale
    float r = imageAspectRatio * scale; //Right
	float l = -r; //Left
    float t = scale; //scale again
	float b = -t; //negative scale

	// Perspective Matrix
    result[0] = 2 * n / (r - l),   result[1] = 0,                 result[2] = (r + l) / (r - l),     result[3] = 0; 
    result[4] = 0,				   result[5] = 2 * n / (t - b),   result[6] = (t + b) / (t - b),     result[7] = 0; 
    result[8] = 0,				   result[9] = 0,				  result[10] = -(f + n) / (f - n),   result[11] = -2 * f * n / (f - n); 
    result[12] = 0,				   result[13] = 0,                result[14] = -1,					 result[15] = 0; 

	return Matrix(result);
}

//ALWAYS CALL THIS FUNCTION BEFORE SENDING A MATRIX TO A SHADER!!!!!!!!!!!!!
Matrix Matrix::getGLFormat(){
	return transpose(); //Convert matrix from row major storage to column major storage
}

//Prints out the result in row major
void Matrix::display() {
    std::cout << setprecision(2) << std::setw(6) << _matrix[0] << " "  << std::setw(6) << _matrix[1]
        << " " << std::setw(6) << _matrix[2]  << " " << std::setw(6) << _matrix[3]  << " " << std::endl

        << std::setw(6) << _matrix[4] << " "  << std::setw(6) << _matrix[5]  << " " << std::setw(6) << 
        _matrix[6]  << " " << std::setw(6) << _matrix[7]  << " " << std::endl

        << std::setw(6) << _matrix[8] << " "  << std::setw(6) << _matrix[9]  << " " << std::setw(6) <<
        _matrix[10] << " " << std::setw(6) << _matrix[11] << " " << std::endl

        << std::setw(6) << _matrix[12] << " " << std::setw(6) << _matrix[13] << " " << std::setw(6) << 
        _matrix[14] << " " << std::setw(6) << _matrix[15] << " " << std::endl << std::endl;
}