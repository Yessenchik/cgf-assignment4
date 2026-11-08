#pragma once

#include "Vec3.h"

#include <cstddef>
#include <vector>

struct Mesh {
    std::vector<Vec3> positions;
    std::vector<Vec3> normals;
    std::vector<unsigned int> indices;

    std::size_t triangleCount() const { return indices.size() / 3; }
};

Mesh makeSphere(float radius, int slices, int stacks);
Mesh makeTorus(float majorRadius, float minorRadius, int rings, int sides);
Mesh makeRoundedCube(float halfSize, float cornerRadius, int divisions);

void drawMesh(const Mesh& mesh, bool withNormals);
