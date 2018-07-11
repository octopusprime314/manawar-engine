#include "Matrix.h"
#include <iostream>
#include <iomanip>
using namespace std;

Matrix::Matrix() {
    //Identity 4x4 homogenous matrix
    _matrix[0] = 1.0, _matrix[1] = 0.0, _matrix[2] = 0.0, _matrix[3] = 0.0;
    _matrix[4] = 0.0, _matrix[5] = 1.0, _matrix[6] = 0.0, _matrix[7] = 0.0;
    _matrix[8] = 0.0, _matrix[9] = 0.0, _matrix[10] = 1.0, _matrix[11] = 0.0;
    _matrix[12] = 0.0, _matrix[13] = 0.0, _matrix[14] = 0.0, _matrix[15] = 1.0;
}

Matrix::Matrix(float* mat) {

    _matrix[0] = mat[0], _matrix[1] = mat[1], _matrix[2] = mat[2], _matrix[3] = mat[3];
    _matrix[4] = mat[4], _matrix[5] = mat[5], _matrix[6] = mat[6], _matrix[7] = mat[7];
    _matrix[8] = mat[8], _matrix[9] = mat[9], _matrix[10] = mat[10], _matrix[11] = mat[11];
    _matrix[12] = mat[12], _matrix[13] = mat[13], _matrix[14] = mat[14], _matrix[15] = mat[15];
}

Matrix::Matrix(double* mat) {

    _matrix[0] = (float)mat[0], _matrix[1] = (float)mat[1], _matrix[2] = (float)mat[2], _matrix[3] = (float)mat[3];
    _matrix[4] = (float)mat[4], _matrix[5] = (float)mat[5], _matrix[6] = (float)mat[6], _matrix[7] = (float)mat[7];
    _matrix[8] = (float)mat[8], _matrix[9] = (float)mat[9], _matrix[10] = (float)mat[10], _matrix[11] = (float)mat[11];
    _matrix[12] = (float)mat[12], _matrix[13] = (float)mat[13], _matrix[14] = (float)mat[14], _matrix[15] = (float)mat[15];
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

Matrix Matrix::inverse() {

    Matrix matrix(_matrix);
    float* mat = matrix.getFlatBuffer();
    float a11 = mat[0];
    float a12 = mat[1];
    float a13 = mat[2];
    float a14 = mat[3];
    float a21 = mat[4];
    float a22 = mat[5];
    float a23 = mat[6];
    float a24 = mat[7];
    float a31 = mat[8];
    float a32 = mat[9];
    float a33 = mat[10];
    float a34 = mat[11];
    float a41 = mat[12];
    float a42 = mat[13];
    float a43 = mat[14];
    float a44 = mat[15];

    float det = (a11 * a22 * a33 * a44) + (a11 * a23 * a34 * a42) + (a11 * a24 * a32 * a43) +
        (a12 * a21 * a34 * a43) + (a12 * a23 * a31 * a44) + (a12 * a24 * a33 * a41) +
        (a13 * a21 * a32 * a44) + (a13 * a22 * a34 * a41) + (a13 * a24 * a31 * a42) +
        (a14 * a21 * a33 * a42) + (a14 * a22 * a31 * a43) + (a14 * a23 * a32 * a41) -
        (a11 * a22 * a34 * a43) - (a11 * a23 * a32 * a44) - (a11 * a24 * a33 * a42) -
        (a12 * a21 * a33 * a44) - (a12 * a23 * a34 * a41) - (a12 * a24 * a31 * a43) -
        (a13 * a21 * a34 * a42) - (a13 * a22 * a31 * a44) - (a13 * a24 * a32 * a41) -
        (a14 * a21 * a32 * a43) - (a14 * a22 * a33 * a41) - (a14 * a23 * a31 * a42);

    //Determinant cannot equal zero
    if (det != 0) {

        float b11 = (a22*a33*a44) + (a23*a34*a42) + (a24*a32*a43) - (a22*a34*a43) - (a23*a32*a44) - (a24*a33*a42);
        float b12 = (a12*a34*a43) + (a13*a32*a44) + (a14*a33*a42) - (a12*a33*a44) - (a13*a34*a42) - (a14*a32*a43);
        float b13 = (a12*a23*a44) + (a13*a24*a42) + (a14*a22*a43) - (a12*a24*a43) - (a13*a22*a44) - (a14*a23*a42);
        float b14 = (a12*a24*a33) + (a13*a22*a34) + (a14*a23*a32) - (a12*a23*a34) - (a13*a24*a32) - (a14*a22*a33);
        float b21 = (a21*a34*a43) + (a23*a31*a44) + (a24*a33*a41) - (a21*a33*a44) - (a23*a34*a41) - (a24*a31*a43);
        float b22 = (a11*a33*a44) + (a13*a34*a41) + (a14*a31*a43) - (a11*a34*a43) - (a13*a31*a44) - (a14*a33*a41);
        float b23 = (a11*a24*a43) + (a13*a21*a44) + (a14*a23*a41) - (a11*a23*a44) - (a13*a24*a41) - (a14*a21*a43);
        float b24 = (a11*a23*a34) + (a13*a24*a31) + (a14*a21*a33) - (a11*a24*a33) - (a13*a21*a34) - (a14*a23*a31);
        float b31 = (a21*a32*a44) + (a22*a34*a41) + (a24*a31*a42) - (a21*a34*a42) - (a22*a31*a44) - (a24*a32*a41);
        float b32 = (a11*a34*a42) + (a12*a31*a44) + (a14*a32*a41) - (a11*a32*a44) - (a12*a34*a41) - (a14*a31*a42);
        float b33 = (a11*a22*a44) + (a12*a24*a41) + (a14*a21*a42) - (a11*a24*a42) - (a12*a21*a44) - (a14*a22*a41);
        float b34 = (a11*a24*a32) + (a12*a21*a34) + (a14*a22*a31) - (a11*a22*a34) - (a12*a24*a31) - (a14*a21*a32);
        float b41 = (a21*a33*a42) + (a22*a31*a43) + (a23*a32*a41) - (a21*a32*a43) - (a22*a33*a41) - (a23*a31*a42);
        float b42 = (a11*a32*a43) + (a12*a33*a41) + (a13*a31*a42) - (a11*a33*a42) - (a12*a31*a43) - (a13*a32*a41);
        float b43 = (a11*a23*a42) + (a12*a21*a43) + (a13*a22*a41) - (a11*a22*a43) - (a12*a23*a41) - (a13*a21*a42);
        float b44 = (a11*a22*a33) + (a12*a23*a31) + (a13*a21*a32) - (a11*a23*a32) - (a12*a21*a33) - (a13*a22*a31);


        mat[0] = b11 / det;
        mat[1] = b12 / det;
        mat[2] = b13 / det;
        mat[3] = b14 / det;
        mat[4] = b21 / det;
        mat[5] = b22 / det;
        mat[6] = b23 / det;
        mat[7] = b24 / det;
        mat[8] = b31 / det;
        mat[9] = b32 / det;
        mat[10] = b33 / det;
        mat[11] = b34 / det;
        mat[12] = b41 / det;
        mat[13] = b42 / det;
        mat[14] = b43 / det;
        mat[15] = b44 / det;
    }
    return matrix;
}

Vector4 Matrix::operator * (Vector4 vec) {
    Vector4 result; //Get underlying vector memory
    float* vector = vec.getFlatBuffer();

    float x = _matrix[0] * vector[0] + _matrix[1] * vector[1] + _matrix[2] * vector[2] + _matrix[3] * vector[3];
    float y = _matrix[4] * vector[0] + _matrix[5] * vector[1] + _matrix[6] * vector[2] + _matrix[7] * vector[3];
    float z = _matrix[8] * vector[0] + _matrix[9] * vector[1] + _matrix[10] * vector[2] + _matrix[11] * vector[3];
    float w = _matrix[12] * vector[0] + _matrix[13] * vector[1] + _matrix[14] * vector[2] + _matrix[15] * vector[3];

    return Vector4(x, y, z, w);
}

Matrix Matrix::operator * (Matrix mat) {

    float result[16]; //Get underlying matrix memory
    float* matBuff = mat.getFlatBuffer();

    result[0] = _matrix[0] * matBuff[0] + _matrix[1] * matBuff[4] + _matrix[2] * matBuff[8] + _matrix[3] * matBuff[12];
    result[1] = _matrix[0] * matBuff[1] + _matrix[1] * matBuff[5] + _matrix[2] * matBuff[9] + _matrix[3] * matBuff[13];
    result[2] = _matrix[0] * matBuff[2] + _matrix[1] * matBuff[6] + _matrix[2] * matBuff[10] + _matrix[3] * matBuff[14];
    result[3] = _matrix[0] * matBuff[3] + _matrix[1] * matBuff[7] + _matrix[2] * matBuff[11] + _matrix[3] * matBuff[15];

    result[4] = _matrix[4] * matBuff[0] + _matrix[5] * matBuff[4] + _matrix[6] * matBuff[8] + _matrix[7] * matBuff[12];
    result[5] = _matrix[4] * matBuff[1] + _matrix[5] * matBuff[5] + _matrix[6] * matBuff[9] + _matrix[7] * matBuff[13];
    result[6] = _matrix[4] * matBuff[2] + _matrix[5] * matBuff[6] + _matrix[6] * matBuff[10] + _matrix[7] * matBuff[14];
    result[7] = _matrix[4] * matBuff[3] + _matrix[5] * matBuff[7] + _matrix[6] * matBuff[11] + _matrix[7] * matBuff[15];

    result[8] = _matrix[8] * matBuff[0] + _matrix[9] * matBuff[4] + _matrix[10] * matBuff[8] + _matrix[11] * matBuff[12];
    result[9] = _matrix[8] * matBuff[1] + _matrix[9] * matBuff[5] + _matrix[10] * matBuff[9] + _matrix[11] * matBuff[13];
    result[10] = _matrix[8] * matBuff[2] + _matrix[9] * matBuff[6] + _matrix[10] * matBuff[10] + _matrix[11] * matBuff[14];
    result[11] = _matrix[8] * matBuff[3] + _matrix[9] * matBuff[7] + _matrix[10] * matBuff[11] + _matrix[11] * matBuff[15];

    result[12] = _matrix[12] * matBuff[0] + _matrix[13] * matBuff[4] + _matrix[14] * matBuff[8] + _matrix[15] * matBuff[12];
    result[13] = _matrix[12] * matBuff[1] + _matrix[13] * matBuff[5] + _matrix[14] * matBuff[9] + _matrix[15] * matBuff[13];
    result[14] = _matrix[12] * matBuff[2] + _matrix[13] * matBuff[6] + _matrix[14] * matBuff[10] + _matrix[15] * matBuff[14];
    result[15] = _matrix[12] * matBuff[3] + _matrix[13] * matBuff[7] + _matrix[14] * matBuff[11] + _matrix[15] * matBuff[15];

    return Matrix(result);
}
Matrix Matrix::operator + (Matrix mat) {

    float result[16]; //Get underlying matrix memory
    float* matBuff = mat.getFlatBuffer();

    result[0] = _matrix[0] + matBuff[0];
    result[1] = _matrix[1] + matBuff[1];
    result[2] = _matrix[2] + matBuff[2];
    result[3] = _matrix[3] + matBuff[3];

    result[4] = _matrix[4] + matBuff[4];
    result[5] = _matrix[5] + matBuff[5];
    result[6] = _matrix[6] + matBuff[6];
    result[7] = _matrix[7] + matBuff[7];

    result[8] = _matrix[8] + matBuff[8];
    result[9] = _matrix[9] + matBuff[9];
    result[10] = _matrix[10] + matBuff[10];
    result[11] = _matrix[11] + matBuff[11];

    result[12] = _matrix[12] + matBuff[12];
    result[13] = _matrix[13] + matBuff[13];
    result[14] = _matrix[14] + matBuff[14];
    result[15] = _matrix[15] + matBuff[15];

    return Matrix(result);
}
Matrix Matrix::operator * (double scale) {
    float result[16]; //Get underlying matrix memory

    result[0] = _matrix[0] * static_cast<float>(scale);
    result[1] = _matrix[1] * static_cast<float>(scale);
    result[2] = _matrix[2] * static_cast<float>(scale);
    result[3] = _matrix[3] * static_cast<float>(scale);

    result[4] = _matrix[4] * static_cast<float>(scale);
    result[5] = _matrix[5] * static_cast<float>(scale);
    result[6] = _matrix[6] * static_cast<float>(scale);
    result[7] = _matrix[7] * static_cast<float>(scale);

    result[8] = _matrix[8] * static_cast<float>(scale);
    result[9] = _matrix[9] * static_cast<float>(scale);
    result[10] = _matrix[10] * static_cast<float>(scale);
    result[11] = _matrix[11] * static_cast<float>(scale);

    result[12] = _matrix[12] * static_cast<float>(scale);
    result[13] = _matrix[13] * static_cast<float>(scale);
    result[14] = _matrix[14] * static_cast<float>(scale);
    result[15] = _matrix[15] * static_cast<float>(scale);

    return Matrix(result);
}
Matrix Matrix::operator * (float scale) {
    float result[16]; //Get underlying matrix memory

    result[0] = _matrix[0] * scale;
    result[1] = _matrix[1] * scale;
    result[2] = _matrix[2] * scale;
    result[3] = _matrix[3] * scale;

    result[4] = _matrix[4] * scale;
    result[5] = _matrix[5] * scale;
    result[6] = _matrix[6] * scale;
    result[7] = _matrix[7] * scale;

    result[8] = _matrix[8] * scale;
    result[9] = _matrix[9] * scale;
    result[10] = _matrix[10] * scale;
    result[11] = _matrix[11] * scale;

    result[12] = _matrix[12] * scale;
    result[13] = _matrix[13] * scale;
    result[14] = _matrix[14] * scale;
    result[15] = _matrix[15] * scale;

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

    result[0] = 1.0, result[1] = 0.0, result[2] = 0.0, result[3] = 0.0;
    result[4] = 0.0, result[5] = cos(theta), result[6] = -sin(theta), result[7] = 0.0;
    result[8] = 0.0, result[9] = sin(theta), result[10] = cos(theta), result[11] = 0.0;
    result[12] = 0.0, result[13] = 0.0, result[14] = 0.0, result[15] = 1.0;

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

    result[0] = cos(theta), result[1] = 0.0, result[2] = sin(theta), result[3] = 0.0;
    result[4] = 0.0, result[5] = 1.0, result[6] = 0.0, result[7] = 0.0;
    result[8] = -sin(theta), result[9] = 0.0, result[10] = cos(theta), result[11] = 0.0;
    result[12] = 0.0, result[13] = 0.0, result[14] = 0.0, result[15] = 1.0;

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

    result[0] = cos(theta), result[1] = -sin(theta), result[2] = 0.0, result[3] = 0.0;
    result[4] = sin(theta), result[5] = cos(theta), result[6] = 0.0, result[7] = 0.0;
    result[8] = 0.0, result[9] = 0.0, result[10] = 1.0, result[11] = 0.0;
    result[12] = 0.0, result[13] = 0.0, result[14] = 0.0, result[15] = 1.0;

    return Matrix(result);
}

//Translation Matrix of a +5 change in X position
//| 1 0 0 5 |
//| 0 1 0 0 |
//| 0 0 1 0 |
//| 0 0 0 1 |
Matrix Matrix::translation(float x, float y, float z) {
    float result[16]; //Get underlying matrix memory

    result[0] = 1.0, result[1] = 0.0, result[2] = 0.0, result[3] = x;
    result[4] = 0.0, result[5] = 1.0, result[6] = 0.0, result[7] = y;
    result[8] = 0.0, result[9] = 0.0, result[10] = 1.0, result[11] = z;
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

    result[0] = scalar, result[1] = 0.0, result[2] = 0.0, result[3] = 0.0;
    result[4] = 0.0, result[5] = scalar, result[6] = 0.0, result[7] = 0.0;
    result[8] = 0.0, result[9] = 0.0, result[10] = scalar, result[11] = 0.0;
    result[12] = 0.0, result[13] = 0.0, result[14] = 0.0, result[15] = 1.0;

    return Matrix(result);
}

//Scale Matrix of 2, 4 and 5
//| 2 0 0 0 |
//| 0 4 0 0 |
//| 0 0 5 0 |
//| 0 0 0 1 |
Matrix Matrix::scale(float x, float y, float z) {
    float result[16]; //Get underlying matrix memory

    result[0] = x, result[1] = 0.0, result[2] = 0.0, result[3] = 0.0;
    result[4] = 0.0, result[5] = y, result[6] = 0.0, result[7] = 0.0;
    result[8] = 0.0, result[9] = 0.0, result[10] = z, result[11] = 0.0;
    result[12] = 0.0, result[13] = 0.0, result[14] = 0.0, result[15] = 1.0;

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

Matrix Matrix::cameraProjection(float angleOfView, float imageAspectRatio, float n, float f) {

    float result[16];

    //Setup components of projection
    float scale = static_cast<float>(tan(angleOfView * 0.5 * PI_OVER_180)) * n; //scale
    float r = imageAspectRatio * scale; //Right
    float l = -r; //Left
    float t = scale; //scale again
    float b = -t; //negative scale

    // Perspective Matrix
    result[0] = 2 * n / (r - l), result[1] = 0, result[2] = (r + l) / (r - l), result[3] = 0;
    result[4] = 0, result[5] = 2 * n / (t - b), result[6] = (t + b) / (t - b), result[7] = 0;
    result[8] = 0, result[9] = 0, result[10] = -(f + n) / (f - n), result[11] = -2 * f * n / (f - n);
    result[12] = 0, result[13] = 0, result[14] = -1, result[15] = 0;

    return Matrix(result);
}

Matrix Matrix::cameraOrtho(float orthoWidth, float orthoHeight, float n, float f) {

    float result[16];

    //Setup components of projection
    float r = orthoWidth / 2.0f;   //Right
    float l = -r;                  //Left
    float t = orthoHeight / 2.0f;  //Top
    float b = -t;                  //Bottom

    // Ortho Matrix
    result[0] = 2.0f / (r - l), result[1] = 0.0f, result[2] = 0.0f, result[3] = -((r + l) / (r - l));
    result[4] = 0.0f, result[5] = 2.0f / (t - b), result[6] = 0.0f, result[7] = -((t + b) / (t - b));
    result[8] = 0.0f, result[9] = 0.0f, result[10] = -2.0f / (f - n), result[11] = -((f + n) / (f - n));
    result[12] = 0.0f, result[13] = 0.0f, result[14] = 0.0f, result[15] = 1.0f;

    return Matrix(result);

}

//Prints out the result in row major
void Matrix::display() {
    std::cout << setprecision(2) << std::setw(6) << _matrix[0] << " " << std::setw(6) << _matrix[1]
        << " " << std::setw(6) << _matrix[2] << " " << std::setw(6) << _matrix[3] << " " << std::endl

        << std::setw(6) << _matrix[4] << " " << std::setw(6) << _matrix[5] << " " << std::setw(6) <<
        _matrix[6] << " " << std::setw(6) << _matrix[7] << " " << std::endl

        << std::setw(6) << _matrix[8] << " " << std::setw(6) << _matrix[9] << " " << std::setw(6) <<
        _matrix[10] << " " << std::setw(6) << _matrix[11] << " " << std::endl

        << std::setw(6) << _matrix[12] << " " << std::setw(6) << _matrix[13] << " " << std::setw(6) <<
        _matrix[14] << " " << std::setw(6) << _matrix[15] << " " << std::endl << std::endl;
}
