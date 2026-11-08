#include "BezierMesh.h"

namespace {

float bernsteinDerivative(int i, float t) {
    float s = 1.0f - t;

    switch (i) {
        case 0: return -3.0f * s * s;
        case 1: return 3.0f * s * s - 6.0f * t * s;
        case 2: return 6.0f * t * s - 3.0f * t * t;
        case 3: return 3.0f * t * t;
        default: return 0.0f;
    }
}

Vec3 surfaceNormal(const BezierPatch& patch, float u, float v) {
    Vec3 du = {0.0f, 0.0f, 0.0f};
    Vec3 dv = {0.0f, 0.0f, 0.0f};

    for (int i = 0; i < BezierPatch::kSize; i++) {
        for (int j = 0; j < BezierPatch::kSize; j++) {
            const Vec3& p = patch.controlPoint(i, j);
            du = add(du, scale(p, BezierPatch::bernstein(i, v) * bernsteinDerivative(j, u)));
            dv = add(dv, scale(p, bernsteinDerivative(i, v) * BezierPatch::bernstein(j, u)));
        }
    }

    return normalize(cross(du, dv));
}

}

BezierMeshData buildBezierMesh(const BezierPatch& patch, int resolution) {
    BezierMeshData mesh;
    int n = resolution + 1;
    mesh.vertices.reserve(n * n);

    for (int i = 0; i < n; i++) {
        float v = (float)i / resolution;
        for (int j = 0; j < n; j++) {
            float u = (float)j / resolution;
            mesh.vertices.push_back({patch.evaluate(u, v), surfaceNormal(patch, u, v), u, v});
        }
    }

    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {
            unsigned int p00 = i * n + j;
            unsigned int p10 = p00 + 1;
            unsigned int p01 = p00 + n;
            unsigned int p11 = p01 + 1;
            mesh.indices.insert(mesh.indices.end(), {p00, p10, p11});
            mesh.indices.insert(mesh.indices.end(), {p00, p11, p01});
        }
    }

    return mesh;
}
