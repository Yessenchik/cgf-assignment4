#include "BezierPatch.h"

namespace {
const Vec3 kInitialPoints[BezierPatch::kSize][BezierPatch::kSize] = {
    {
        {-1.5f, -1.5f, 0.0f},
        {-0.5f, -1.5f, 0.0f},
        { 0.5f, -1.5f, 0.0f},
        { 1.5f, -1.5f, 0.0f}
    },
    {
        {-1.5f, -0.5f, 0.0f},
        {-0.5f, -0.5f, 1.5f},
        { 0.5f, -0.5f, 1.5f},
        { 1.5f, -0.5f, 0.0f}
    },
    {
        {-1.5f,  0.5f, 0.0f},
        {-0.5f,  0.5f, 1.5f},
        { 0.5f,  0.5f, 1.5f},
        { 1.5f,  0.5f, 0.0f}
    },
    {
        {-1.5f,  1.5f, 0.0f},
        {-0.5f,  1.5f, 0.0f},
        { 0.5f,  1.5f, 0.0f},
        { 1.5f,  1.5f, 0.0f}
    }
};
}

BezierPatch::BezierPatch() {
    for (int i = 0; i < kSize; i++) {
        for (int j = 0; j < kSize; j++) {
            points_[i][j] = kInitialPoints[i][j];
        }
    }
}

float BezierPatch::bernstein(int i, float t) {
    float s = 1.0f - t;

    switch (i) {
        case 0: return s * s * s;
        case 1: return 3.0f * t * s * s;
        case 2: return 3.0f * t * t * s;
        case 3: return t * t * t;
        default: return 0.0f;
    }
}

Vec3 BezierPatch::evaluate(float u, float v) const {
    Vec3 result = {0.0f, 0.0f, 0.0f};

    for (int i = 0; i < kSize; i++) {
        for (int j = 0; j < kSize; j++) {
            float weight =
                bernstein(i, v) *
                bernstein(j, u);

            result.x += points_[i][j].x * weight;
            result.y += points_[i][j].y * weight;
            result.z += points_[i][j].z * weight;
        }
    }

    return result;
}

void BezierPatch::selectNext() {
    selected_ = (selected_ + 1) % kPointCount;
}

void BezierPatch::moveSelected(float dx, float dy, float dz) {
    int row = selected_ / kSize;
    int col = selected_ % kSize;

    points_[row][col].x += dx;
    points_[row][col].y += dy;
    points_[row][col].z += dz;
}

void BezierPatch::increaseResolution() {
    if (resolution_ < kMaxResolution) {
        resolution_ += 1;
    }
}

void BezierPatch::decreaseResolution() {
    if (resolution_ > kMinResolution) {
        resolution_ -= 1;
    }
}
