#include "Vector4.h"
#include <iostream>
#include <iomanip>
using namespace std;

Vector4::Vector4() {
    _vec[0] = 0.0;
    _vec[1] = 0.0;
    _vec[2] = 0.0;
    _vec[3] = 1.0;
}
Vector4::Vector4(float x, float y, float z, float w) {
    _vec[0] = x;
    _vec[1] = y;
    _vec[2] = z;
    _vec[3] = w;
}

Vector4::Vector4(const Vector4& other) {
    const float* vector = other._vec;
    _vec[0] = vector[0];
    _vec[1] = vector[1];
    _vec[2] = vector[2];
    _vec[3] = vector[3];
}

float* Vector4::getFlatBuffer() {
    return _vec;
}

float Vector4::getMagnitude() {
    return sqrtf((_vec[0] * _vec[0]) + (_vec[1] * _vec[1]) + (_vec[2] * _vec[2]));
}

Vector4 Vector4::operator / (float scale) {
    Vector4 result;
    float* vector = result.getFlatBuffer();
    vector[0] = _vec[0] / scale;
    vector[1] = _vec[1] / scale;
    vector[2] = _vec[2] / scale;
    return result;
}

Vector4 Vector4::operator * (float scale) {
    Vector4 result;
    float* vector = result.getFlatBuffer();
    vector[0] = _vec[0] * scale;
    vector[1] = _vec[1] * scale;
    vector[2] = _vec[2] * scale;
    return result;
}

Vector4 Vector4::operator * (Vector4 other) {
    Vector4 result;
    float* vector = result.getFlatBuffer();
    vector[0] = _vec[0] * other.getx();
    vector[1] = _vec[1] * other.gety();
    vector[2] = _vec[2] * other.getz();
    return result;
}


Vector4 Vector4::operator + (Vector4 other) {
    Vector4 result;
    float* vector = result.getFlatBuffer();
    float* vector2 = other.getFlatBuffer();
    vector[0] = _vec[0] + vector2[0];
    vector[1] = _vec[1] + vector2[1];
    vector[2] = _vec[2] + vector2[2];
    return result;
}

Vector4& Vector4::operator += (Vector4 other) {
    float* this2 = this->getFlatBuffer();
    float* other2 = other.getFlatBuffer();
    this2[0] = this2[0] + other2[0];
    this2[1] = this2[1] + other2[1];
    this2[2] = this2[2] + other2[2];
    return *this;
}

Vector4 Vector4::operator - (Vector4 other) {
    Vector4 result;
    float* vector = result.getFlatBuffer();
    float* vector2 = other.getFlatBuffer();
    vector[0] = _vec[0] - vector2[0];
    vector[1] = _vec[1] - vector2[1];
    vector[2] = _vec[2] - vector2[2];
    return result;
}

Vector4 Vector4::operator - () {
    Vector4 result;
    float* vector = result.getFlatBuffer();
    vector[0] = -_vec[0];
    vector[1] = -_vec[1];
    vector[2] = -_vec[2];
    return result;
}


Vector4 Vector4::crossProduct(Vector4 other) {
    Vector4 result;
    float* vector = result.getFlatBuffer();
    float* vector2 = other.getFlatBuffer();
    //x1 y1 z1
    //x2 y2 z2
    //x = y1*z2 - z1*y2
    //y = x1*z2 - z1*x2
    //z = x1*y2 - y1*x2
    //result = x - y + z
    vector[0] = (_vec[1] * vector2[2]) - (_vec[2] * vector2[1]);
    vector[1] = -(_vec[0] * vector2[2]) + (_vec[2] * vector2[0]);
    vector[2] = (_vec[0] * vector2[1]) - (_vec[1] * vector2[0]);
    vector[3] = 0.f;
    return result;
}

float Vector4::dotProduct(Vector4 other) {
    float result;
    float* vector2 = other.getFlatBuffer();
    result = (_vec[0] * vector2[0]) + (_vec[1] * vector2[1]) + (_vec[2] * vector2[2]);
    return result;
}

void Vector4::normalize() {
    Vector4 result;
    float* vector = result.getFlatBuffer();
    float mag = getMagnitude();
    _vec[0] /= mag;
    _vec[1] /= mag;
    _vec[2] /= mag;
    _vec[3] /= mag;
}

//Prints out the result in row major
void Vector4::display() {
    std::cout << setprecision(6) << std::setw(6) << _vec[0] << " " << std::setw(6) << _vec[1]
        << " " << std::setw(6) << _vec[2] << " " << std::setw(6) << _vec[3] << " " << std::endl;
}

float Vector4::getx() {
    return _vec[0];
}

float Vector4::gety() {
    return _vec[1];
}

float Vector4::getz() {
    return _vec[2];
}

float Vector4::getw() {
    return _vec[3];
}

bool Vector4::operator == (Vector4 other) {
    float* ovec = other.getFlatBuffer();
    if (_vec[0] == ovec[0] && _vec[1] == ovec[1] && _vec[2] == ovec[2] && _vec[3] == ovec[3]) {
        return true;
    }
    else {
        return false;
    }
}

bool Vector4::operator != (Vector4 other) {
    float* ovec = other.getFlatBuffer();
    if (_vec[0] != ovec[0] || _vec[1] != ovec[1] || _vec[2] != ovec[2] || _vec[3] != ovec[3]) {
        return true;
    }
    else {
        return false;
    }
}

std::ostream& operator << (std::ostream& output, Vector4 &other) {
    float* vec = other.getFlatBuffer();
    output << setprecision(2) << std::setw(6) << vec[0] << " " << std::setw(6) << vec[1]
        << " " << std::setw(6) << vec[2] << " " << std::setw(6) << vec[3] << " " << std::endl;
    return output;
}
