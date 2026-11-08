#pragma once

struct Vec3 {
    float x, y, z;
};

Vec3 subtract(Vec3 a, Vec3 b);
Vec3 cross(Vec3 a, Vec3 b);
Vec3 normalize(Vec3 v);

