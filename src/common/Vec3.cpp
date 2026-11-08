#include "Vec3.h"
#include <cmath>

Vec3 subtract(Vec3 a, Vec3 b) {
    return {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
}

Vec3 cross(Vec3 a, Vec3 b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

Vec3 normalize(Vec3 v) {
    float length = std::sqrt(
        v.x * v.x +
        v.y * v.y +
        v.z * v.z
    );

    if (length < 0.00001f) {
        return {0.0f, 0.0f, 1.0f};
    }

    return {
        v.x / length,
        v.y / length,
        v.z / length
    };
}
