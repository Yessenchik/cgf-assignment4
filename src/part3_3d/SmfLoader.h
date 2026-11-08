#pragma once

#include "GpuMesh.h"

#include <string>
#include <vector>

struct SmfModel {
    std::vector<MeshVertex> vertices;
    std::vector<unsigned int> indices;
    Vec3 center = {0.0f, 0.0f, 0.0f};
    float radius = 1.0f;
};

bool loadSmf(const std::string& path, SmfModel& model, std::string& error);
