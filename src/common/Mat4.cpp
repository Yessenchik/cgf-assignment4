#include "Mat4.h"

#include <cmath>

namespace {
constexpr float PI = 3.14159265359f;
}

Mat4 Mat4::identity() {
    Mat4 r;
    r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0f;
    return r;
}

Mat4 multiply(const Mat4& a, const Mat4& b) {
    Mat4 r;
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            float sum = 0.0f;
            for (int k = 0; k < 4; k++) {
                sum += a.at(row, k) * b.at(k, col);
            }
            r.at(row, col) = sum;
        }
    }
    return r;
}

Vec3 transformPoint(const Mat4& m, Vec3 p) {
    return {
        m.at(0, 0) * p.x + m.at(0, 1) * p.y + m.at(0, 2) * p.z + m.at(0, 3),
        m.at(1, 0) * p.x + m.at(1, 1) * p.y + m.at(1, 2) * p.z + m.at(1, 3),
        m.at(2, 0) * p.x + m.at(2, 1) * p.y + m.at(2, 2) * p.z + m.at(2, 3)
    };
}

Mat4 translation(Vec3 t) {
    Mat4 r = Mat4::identity();
    r.at(0, 3) = t.x;
    r.at(1, 3) = t.y;
    r.at(2, 3) = t.z;
    return r;
}

Mat4 uniformScale(float s) {
    Mat4 r = Mat4::identity();
    r.at(0, 0) = r.at(1, 1) = r.at(2, 2) = s;
    return r;
}

Mat4 rotationX(float degrees) {
    float a = degrees * PI / 180.0f;
    Mat4 r = Mat4::identity();
    r.at(1, 1) = std::cos(a);
    r.at(1, 2) = -std::sin(a);
    r.at(2, 1) = std::sin(a);
    r.at(2, 2) = std::cos(a);
    return r;
}

Mat4 lookAt(Vec3 eye, Vec3 center, Vec3 up) {
    Vec3 forward = normalize(subtract(center, eye));
    Vec3 right = normalize(cross(forward, up));
    Vec3 trueUp = cross(right, forward);

    Mat4 r = Mat4::identity();
    r.at(0, 0) = right.x;    r.at(0, 1) = right.y;    r.at(0, 2) = right.z;
    r.at(1, 0) = trueUp.x;   r.at(1, 1) = trueUp.y;   r.at(1, 2) = trueUp.z;
    r.at(2, 0) = -forward.x; r.at(2, 1) = -forward.y; r.at(2, 2) = -forward.z;
    r.at(0, 3) = -dot(right, eye);
    r.at(1, 3) = -dot(trueUp, eye);
    r.at(2, 3) = dot(forward, eye);
    return r;
}

Mat4 perspective(float fovyDegrees, float aspect, float zNear, float zFar) {
    float f = 1.0f / std::tan(fovyDegrees * PI / 360.0f);
    Mat4 r;
    r.at(0, 0) = f / aspect;
    r.at(1, 1) = f;
    r.at(2, 2) = (zFar + zNear) / (zNear - zFar);
    r.at(2, 3) = (2.0f * zFar * zNear) / (zNear - zFar);
    r.at(3, 2) = -1.0f;
    return r;
}

Mat3 normalMatrix(const Mat4& m) {
    float a = m.at(0, 0), b = m.at(0, 1), c = m.at(0, 2);
    float d = m.at(1, 0), e = m.at(1, 1), f = m.at(1, 2);
    float g = m.at(2, 0), h = m.at(2, 1), i = m.at(2, 2);

    float det = a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
    Mat3 r;
    if (std::fabs(det) < 1e-12f) {
        r.at(0, 0) = r.at(1, 1) = r.at(2, 2) = 1.0f;
        return r;
    }
    float inv = 1.0f / det;

    r.at(0, 0) = (e * i - f * h) * inv;
    r.at(0, 1) = -(d * i - f * g) * inv;
    r.at(0, 2) = (d * h - e * g) * inv;
    r.at(1, 0) = -(b * i - c * h) * inv;
    r.at(1, 1) = (a * i - c * g) * inv;
    r.at(1, 2) = -(a * h - b * g) * inv;
    r.at(2, 0) = (b * f - c * e) * inv;
    r.at(2, 1) = -(a * f - c * d) * inv;
    r.at(2, 2) = (a * e - b * d) * inv;
    return r;
}
