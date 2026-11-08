#pragma once

#include "GL3.h"
#include "Vec3.h"

#include <vector>

struct MeshVertex {
    Vec3 position;
    Vec3 normal;
    float u, v;
};

class GpuMesh {
public:
    GpuMesh() = default;
    GpuMesh(const GpuMesh&) = delete;
    GpuMesh& operator=(const GpuMesh&) = delete;

    void upload(const std::vector<MeshVertex>& vertices,
                const std::vector<unsigned int>& indices);
    void draw() const;
    void destroy();

private:
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    GLuint ebo_ = 0;
    GLsizei indexCount_ = 0;
};
