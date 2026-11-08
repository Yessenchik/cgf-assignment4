#pragma once

#include "Vec3.h"

class BezierPatch {
public:
    static constexpr int kSize = 4;
    static constexpr int kPointCount = kSize * kSize;
    static constexpr int kDefaultResolution = 10;
    static constexpr int kMinResolution = 2;
    static constexpr int kMaxResolution = 50;

    BezierPatch();

    static float bernstein(int i, float t);

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
