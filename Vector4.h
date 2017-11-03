#pragma once

class Vector4 {
    float _vec[4];
public:
    
    Vector4();
    Vector4(float x, float y, float z, float w);
    float* getFlatBuffer();
    void display();
	Vector4 operator / (float scale);
	Vector4 operator + (Vector4 other);
	float getx();
	float gety();
	float getz();
	float getw();
};