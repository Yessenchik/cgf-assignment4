#pragma once

#include "Vec3.h"

// Bicubic Bezier patch defined by a 4x4 grid of control points,
// plus the editing state (selected point, tessellation resolution).
class BezierPatch {
public:
    static constexpr int kSize = 4;
    static constexpr int kPointCount = kSize * kSize;
    static constexpr int kDefaultResolution = 10;
    static constexpr int kMinResolution = 2;
    static constexpr int kMaxResolution = 50;

    BezierPatch();

    // Cubic Bernstein basis polynomial B_i(t), i in [0, 3].
    static float bernstein(int i, float t);

    // Evaluates the surface point at (u, v) in [0, 1]^2.
    Vec3 evaluate(float u, float v) const;

    const Vec3& controlPoint(int row, int col) const { return points_[row][col]; }

    int selectedIndex() const { return selected_; }
    void selectNext();
    void moveSelected(float dx, float dy, float dz);

    int resolution() const { return resolution_; }
    void increaseResolution();
    void decreaseResolution();

private:
    Vec3 points_[kSize][kSize];
    int selected_ = 0;
    int resolution_ = kDefaultResolution;
};
