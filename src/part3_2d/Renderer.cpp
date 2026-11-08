#include "Renderer.h"

#include "BezierMesh.h"
#include "BezierPatch.h"
#include "GpuMesh.h"
#include "ShaderProgram.h"
#include "Texture2D.h"
#include "ViewSetup.h"

#include <algorithm>
#include <cstdio>

namespace {

const Camera* gCamera = nullptr;
ShaderProgram gShader;
GpuMesh gMesh;
Texture2D gTexture;

int gWidth = 1;
int gHeight = 1;
bool gWireframe = false;

void printMeshStats(const BezierMeshData& mesh) {
    float minU = 1, maxU = 0, minV = 1, maxV = 0;
    for (const MeshVertex& v : mesh.vertices) {
        minU = std::min(minU, v.u); maxU = std::max(maxU, v.u);
        minV = std::min(minV, v.v); maxV = std::max(maxV, v.v);
    }
    std::printf("Bezier mesh %dx%d: %zu vertices, %zu triangles, u in [%.0f, %.0f], v in [%.0f, %.0f]\n",
                Renderer::kResolution, Renderer::kResolution,
                mesh.vertices.size(), mesh.indices.size() / 3, minU, maxU, minV, maxV);
}

}

namespace Renderer {

bool init(const Camera* camera) {
    gCamera = camera;

    std::printf("OpenGL %s, GLSL %s\n",
                (const char*)glGetString(GL_VERSION),
                (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

    if (!gShader.load("part3_2d.vert", "part3_2d.frag")) {
        return false;
    }

    BezierPatch patch;
    BezierMeshData mesh = buildBezierMesh(patch, kResolution);
    printMeshStats(mesh);
    gMesh.upload(mesh.vertices, mesh.indices);

    RgbImage image = generatePatchImage(512);
    gTexture.upload(image);
    std::printf("Texture %dx%d RGB, mipmapped\n", image.width, image.height);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
    return true;
}

void shutdown() {
    gTexture.destroy();
    gMesh.destroy();
    gShader.destroy();
}

void reshape(int width, int height) {
    gWidth = std::max(1, width);
    gHeight = std::max(1, height);
    glViewport(0, 0, gWidth, gHeight);
}

void drawFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, gWireframe ? GL_LINE : GL_FILL);

    ViewSetup view = computeView(*gCamera, gWidth, gHeight);
    Mat4 model = Mat4::identity();

    gShader.use();
    gShader.setMat4("uModel", model);
    gShader.setMat4("uView", view.view);
    gShader.setMat4("uProjection", view.projection);
    gShader.setMat3("uNormalMatrix", normalMatrix(model));
    gShader.setVec3("uCameraPosition", view.eye);
    gShader.setVec3("uLightPosition", view.light);
    gShader.setVec3("uLightColor", {1.0f, 1.0f, 1.0f});
    gShader.setFloat("uAmbient", 0.18f);
    gShader.setVec3("uSpecularColor", {0.6f, 0.6f, 0.6f});
    gShader.setFloat("uShininess", 40.0f);

    gTexture.bind(0);
    gShader.setInt("uDiffuseTexture", 0);

    gMesh.draw();
    glUseProgram(0);
}

void display() {
    drawFrame();
    glutSwapBuffers();

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::fprintf(stderr, "OpenGL error 0x%04X\n", error);
    }
}

bool toggleWireframe() {
    gWireframe = !gWireframe;
    return gWireframe;
}

}
