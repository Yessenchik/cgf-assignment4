#include "Mesh.h"

#include <OpenGL/gl.h>
#include <algorithm>
#include <cmath>

namespace {

constexpr float PI = 3.14159265359f;

void addGridIndices(Mesh& mesh, unsigned int base, int rows, int cols) {
    unsigned int stride = cols + 1;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            unsigned int a = base + r * stride + c;
            unsigned int b = a + stride;
            mesh.indices.insert(mesh.indices.end(), {a, b, a + 1});
            mesh.indices.insert(mesh.indices.end(), {a + 1, b, b + 1});
        }
    }
}

float clampf(float v, float lo, float hi) {
    return std::max(lo, std::min(hi, v));
}

}

Mesh makeSphere(float radius, int slices, int stacks) {
    Mesh mesh;

    for (int st = 0; st <= stacks; st++) {
        float phi = PI * st / stacks;
        for (int sl = 0; sl <= slices; sl++) {
            float theta = 2.0f * PI * sl / slices;
            Vec3 n = {
                std::sin(phi) * std::cos(theta),
                std::sin(phi) * std::sin(theta),
                std::cos(phi)
            };
            mesh.normals.push_back(n);
            mesh.positions.push_back(scale(n, radius));
        }
    }

    addGridIndices(mesh, 0, stacks, slices);
    return mesh;
}

Mesh makeTorus(float majorRadius, float minorRadius, int rings, int sides) {
    Mesh mesh;

    for (int i = 0; i <= rings; i++) {
        float u = 2.0f * PI * i / rings;
        Vec3 center = {majorRadius * std::cos(u), majorRadius * std::sin(u), 0.0f};
        for (int j = 0; j <= sides; j++) {
            float v = 2.0f * PI * j / sides;
            Vec3 n = {
                std::cos(v) * std::cos(u),
                std::cos(v) * std::sin(u),
                std::sin(v)
            };
            mesh.normals.push_back(n);
            mesh.positions.push_back(add(center, scale(n, minorRadius)));
        }
    }

    addGridIndices(mesh, 0, rings, sides);
    return mesh;
}

Mesh makeRoundedCube(float halfSize, float cornerRadius, int divisions) {
    struct Face { Vec3 n, u, v; };
    const Face faces[6] = {
        {{ 1, 0, 0}, {0, 1, 0}, {0, 0, 1}},
        {{-1, 0, 0}, {0, 0, 1}, {0, 1, 0}},
        {{ 0, 1, 0}, {0, 0, 1}, {1, 0, 0}},
        {{ 0,-1, 0}, {1, 0, 0}, {0, 0, 1}},
        {{ 0, 0, 1}, {1, 0, 0}, {0, 1, 0}},
        {{ 0, 0,-1}, {0, 1, 0}, {1, 0, 0}},
    };

    float inner = halfSize - cornerRadius;
    Mesh mesh;

    for (const Face& f : faces) {
        unsigned int base = (unsigned int)mesh.positions.size();
        for (int r = 0; r <= divisions; r++) {
            float t = -1.0f + 2.0f * r / divisions;
            for (int c = 0; c <= divisions; c++) {
                float s = -1.0f + 2.0f * c / divisions;
                Vec3 onCube = scale(add(f.n, add(scale(f.u, s), scale(f.v, t))), halfSize);
                Vec3 core = {
                    clampf(onCube.x, -inner, inner),
                    clampf(onCube.y, -inner, inner),
                    clampf(onCube.z, -inner, inner)
                };
                Vec3 n = normalize(subtract(onCube, core));
                mesh.normals.push_back(n);
                mesh.positions.push_back(add(core, scale(n, cornerRadius)));
            }
        }
        unsigned int stride = divisions + 1;
        for (int r = 0; r < divisions; r++) {
            for (int c = 0; c < divisions; c++) {
                unsigned int a = base + r * stride + c;
                mesh.indices.insert(mesh.indices.end(), {a, a + 1, a + stride + 1});
                mesh.indices.insert(mesh.indices.end(), {a, a + stride + 1, a + stride});
            }
        }
    }

    return mesh;
}

void drawMesh(const Mesh& mesh, bool withNormals) {
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(Vec3), mesh.positions.data());

    if (withNormals) {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, sizeof(Vec3), mesh.normals.data());
    }

    glDrawElements(GL_TRIANGLES, (GLsizei)mesh.indices.size(),
                   GL_UNSIGNED_INT, mesh.indices.data());

    if (withNormals) {
        glDisableClientState(GL_NORMAL_ARRAY);
    }
    glDisableClientState(GL_VERTEX_ARRAY);
}
