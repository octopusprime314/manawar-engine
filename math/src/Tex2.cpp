#include "Tex2.h"
#include <iostream>
#include <iomanip>
using namespace std;

Tex2::Tex2() {
    _st[0] = 0.0;
    _st[1] = 0.0;
}
Tex2::Tex2(float s, float t) {
    _st[0] = s;
    _st[1] = t;
}

float* Tex2::getFlatBuffer() {
    return _st;
}

Tex2 Tex2::operator / (float scale) {
    Tex2 result;
    float* st = result.getFlatBuffer();
    st[0]     = _st[0] / scale;
    st[1]     = _st[1] / scale;
    return result;
}

Tex2 Tex2::operator + (Tex2 other) {
    Tex2 result;
    float* st  = result.getFlatBuffer();
    float* st2 = other.getFlatBuffer();
    st[0]      = _st[0] + st2[0];
    st[1]      = _st[1] + st2[1];
    return result;
}

//Prints out the result in row major
void Tex2::display() {
    std::cout << setprecision(2) << std::setw(6) <<
                 _st[0] << " "   << std::setw(6) <<
                 _st[1] << std::endl;
}

float Tex2::gets() {
    return _st[0];
}

float Tex2::gett() {
    return _st[1];
}
