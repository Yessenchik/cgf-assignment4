#pragma once

#include "Vec3.h"

#include <string>
#include <vector>

class BezierPatch {
public:
    static constexpr int kSize = 4;
    static constexpr int kPointCount = kSize * kSize;
    static constexpr int kDefaultResolution = 10;
    static constexpr int kMinResolution = 2;
    static constexpr int kMaxResolution = 50;

    BezierPatch();

    bool loadFromFile(const std::string& path, std::string& error);

    static float bernstein(int i, float t);

    Vec3 evaluate(float u, float v) const;

    const Vec3& controlPoint(int row, int col) const { return points_[row][col]; }

    int selectedIndex() const { return selected_; }
    const Vec3& selectedPoint() const;
    void selectNext();
    void moveSelected(float dx, float dy, float dz);

    int resolution() const { return resolution_; }
    bool increaseResolution();
    bool decreaseResolution();
    int triangleCount() const { return 2 * resolution_ * resolution_; }

    const std::vector<Vec3>& vertices() const { return vertices_; }

private:
    void rebuildMesh();

    Vec3 points_[kSize][kSize];
    int selected_ = 0;
    int resolution_ = kDefaultResolution;
    std::vector<Vec3> vertices_;
};
