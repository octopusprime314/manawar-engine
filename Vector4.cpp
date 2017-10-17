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

float* Vector4::getFlatBuffer() {
    return _vec;
}

Vector4 Vector4::operator / (float scale){
	Vector4 result;
	float* vector = result.getFlatBuffer();
	vector[0] = vector[0] / scale;
	vector[1] = vector[1] / scale;
	vector[2] = vector[2] / scale;
	return result;
}

//Prints out the result in row major
void Vector4::display() {
    std::cout << setprecision(2) << std::setw(6) << _vec[0] << " " << std::setw(6) << _vec[1]
        << " " << std::setw(6) << _vec[2] << " " << std::setw(6) << _vec[3] << " " << std::endl;
}