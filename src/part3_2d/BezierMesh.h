#pragma once

#include "BezierPatch.h"
#include "GpuMesh.h"

#include <vector>

struct BezierMeshData {
    std::vector<MeshVertex> vertices;
    std::vector<unsigned int> indices;
};

BezierMeshData buildBezierMesh(const BezierPatch& patch, int resolution);
