#include "Renderer.h"

#include "GpuMesh.h"
#include "ShaderProgram.h"
#include "SmfLoader.h"
#include "ViewSetup.h"

#include <algorithm>
#include <cstdio>

namespace {

const Camera* gCamera = nullptr;
ShaderProgram gShader;
GpuMesh gMesh;
Mat4 gModel = Mat4::identity();

int gWidth = 1;
int gHeight = 1;
bool gWireframe = false;

}

namespace Renderer {

bool init(const Camera* camera, const std::string& modelFile) {
    gCamera = camera;

    std::printf("OpenGL %s, GLSL %s\n",
                (const char*)glGetString(GL_VERSION),
                (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

    SmfModel model;
    std::string error;
    if (!loadSmf(modelFile, model, error)) {
        std::fprintf(stderr, "Failed to load model: %s\n", error.c_str());
        return false;
    }

    if (!gShader.load("part3_3d.vert", "part3_3d.frag")) {
        return false;
    }

    gMesh.upload(model.vertices, model.indices);

    gModel = multiply(rotationX(90.0f),
                      multiply(uniformScale(1.0f / model.radius),
                               translation(scale(model.center, -1.0f))));

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
    return true;
}

void shutdown() {
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

    gShader.use();
    gShader.setMat4("uModel", gModel);
    gShader.setMat4("uView", view.view);
    gShader.setMat4("uProjection", view.projection);
    gShader.setMat3("uNormalMatrix", normalMatrix(gModel));
    gShader.setVec3("uCameraPosition", view.eye);
    gShader.setVec3("uLightPosition", view.light);
    gShader.setVec3("uLightColor", {1.0f, 1.0f, 1.0f});
    gShader.setFloat("uAmbient", 0.18f);
    gShader.setVec3("uSpecularColor", {0.7f, 0.7f, 0.7f});
    gShader.setFloat("uShininess", 60.0f);
    gShader.setFloat("uTextureScale", 1.3f);

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
