#include "Texture2.h"
#include <iostream>
#include <iomanip>
using namespace std;

Texture2::Texture2() {
    _st[0] = 0.0;
    _st[1] = 0.0;
}
Texture2::Texture2(float s, float t) {
    _st[0] = s;
    _st[1] = t;
}

float* Texture2::getFlatBuffer() {
    return _st;
}

Texture2 Texture2::operator / (float scale) {
    Texture2 result;
    float* st = result.getFlatBuffer();
    st[0] = _st[0] / scale;
    st[1] = _st[1] / scale;
    return result;
}

Texture2 Texture2::operator + (Texture2 other) {
    Texture2 result;
    float* st = result.getFlatBuffer();
    float* st2 = other.getFlatBuffer();
    st[0] = _st[0] + st2[0];
    st[1] = _st[1] + st2[1];
    return result;
}

//Prints out the result in row major
void Texture2::display() {
    std::cout << setprecision(2) << std::setw(6) << _st[0] << " " << std::setw(6) << _st[1] << std::endl;
}

float Texture2::gets() {
    return _st[0];
}

float Texture2::gett() {
    return _st[1];
}