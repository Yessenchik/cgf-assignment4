#include "BezierPatch.h"

#include <fstream>
#include <sstream>

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

    rebuildMesh();
}

bool BezierPatch::loadFromFile(const std::string& path, std::string& error) {
    std::ifstream file(path);
    if (!file) {
        error = "cannot open " + path;
        return false;
    }

    Vec3 loaded[kSize][kSize];
    int count = 0;
    std::string line;

    while (count < kPointCount && std::getline(file, line)) {
        std::istringstream in(line);
        Vec3 p;
        if (!(in >> p.x >> p.y >> p.z)) {
            continue;
        }
        loaded[count / kSize][count % kSize] = p;
        count++;
    }

    if (count != kPointCount) {
        error = "expected 16 points, found " + std::to_string(count);
        return false;
    }

    for (int i = 0; i < kSize; i++) {
        for (int j = 0; j < kSize; j++) {
            points_[i][j] = loaded[i][j];
        }
    }

    rebuildMesh();
    return true;
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

void BezierPatch::rebuildMesh() {
    int n = resolution_ + 1;
    vertices_.resize(n * n);

    for (int i = 0; i < n; i++) {
        float v = (float)i / resolution_;
        for (int j = 0; j < n; j++) {
            float u = (float)j / resolution_;
            vertices_[i * n + j] = evaluate(u, v);
        }
    }
}

const Vec3& BezierPatch::selectedPoint() const {
    return points_[selected_ / kSize][selected_ % kSize];
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

    rebuildMesh();
}

bool BezierPatch::increaseResolution() {
    if (resolution_ >= kMaxResolution) {
        return false;
    }
    resolution_ += 1;
    rebuildMesh();
    return true;
}

bool BezierPatch::decreaseResolution() {
    if (resolution_ <= kMinResolution) {
        return false;
    }
    resolution_ -= 1;
    rebuildMesh();
    return true;
}
