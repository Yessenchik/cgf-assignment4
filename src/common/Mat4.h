#pragma once

#include "Vec3.h"

struct Mat4 {
    float m[16] = {0};

    static Mat4 identity();

    float& at(int row, int col) { return m[col * 4 + row]; }
    float at(int row, int col) const { return m[col * 4 + row]; }
    const float* data() const { return m; }
};

struct Mat3 {
    float m[9] = {0};

    float& at(int row, int col) { return m[col * 3 + row]; }
    float at(int row, int col) const { return m[col * 3 + row]; }
    const float* data() const { return m; }
};

Mat4 multiply(const Mat4& a, const Mat4& b);
Vec3 transformPoint(const Mat4& m, Vec3 p);

Mat4 translation(Vec3 t);
Mat4 uniformScale(float s);
Mat4 rotationX(float degrees);

Mat4 lookAt(Vec3 eye, Vec3 center, Vec3 up);
Mat4 perspective(float fovyDegrees, float aspect, float zNear, float zFar);

Mat3 normalMatrix(const Mat4& m);
