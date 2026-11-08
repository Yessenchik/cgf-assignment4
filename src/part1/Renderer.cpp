#include "Renderer.h"

#include "BezierPatch.h"
#include "Camera.h"

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <cmath>

namespace {

const Camera* gCamera = nullptr;
const BezierPatch* gPatch = nullptr;

float calculateDiffuse(Vec3 a, Vec3 b, Vec3 c, Vec3 lightDir) {
    Vec3 edge1 = subtract(b, a);
    Vec3 edge2 = subtract(c, a);

    Vec3 normal = normalize(cross(edge1, edge2));

    return 0.2f + 0.8f * std::fmax(0.0f, dot(normal, lightDir));
}

void drawShadedTriangle(Vec3 a, Vec3 b, Vec3 c, Vec3 lightDir) {
    float intensity = calculateDiffuse(a, b, c, lightDir);

    glColor3f(
        0.2f * intensity,
        0.6f * intensity,
        0.9f * intensity
    );

    glVertex3f(a.x, a.y, a.z);
    glVertex3f(b.x, b.y, b.z);
    glVertex3f(c.x, c.y, c.z);
}

void drawAxes() {
    glBegin(GL_LINES);

    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0, 0, 0);
    glVertex3f(2, 0, 0);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 2, 0);

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 2);

    glEnd();
}

void drawBezierPatch() {
    const std::vector<Vec3>& grid = gPatch->vertices();
    int resolution = gPatch->resolution();
    int n = resolution + 1;

    Vec3 lightDir = gCamera->direction();

    glBegin(GL_TRIANGLES);

    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {
            const Vec3& p00 = grid[i * n + j];
            const Vec3& p10 = grid[i * n + j + 1];
            const Vec3& p01 = grid[(i + 1) * n + j];
            const Vec3& p11 = grid[(i + 1) * n + j + 1];

            drawShadedTriangle(p00, p10, p11, lightDir);
            drawShadedTriangle(p00, p11, p01, lightDir);
        }
    }

    glEnd();
}

void drawControlPoints() {
    const BezierPatch& patch = *gPatch;

    glDisable(GL_DEPTH_TEST);

    for (int i = 0; i < BezierPatch::kSize; i++) {
        for (int j = 0; j < BezierPatch::kSize; j++) {
            int index = i * BezierPatch::kSize + j;
            bool selected = (index == patch.selectedIndex());

            if (selected) {
                glPointSize(16.0f);
                glColor3f(1.0f, 0.2f, 0.2f);
            } else {
                glPointSize(10.0f);
                glColor3f(1.0f, 0.8f, 0.0f);
            }

            const Vec3& p = patch.controlPoint(i, j);

            glBegin(GL_POINTS);
            glVertex3f(p.x, p.y, p.z);
            glEnd();
        }
    }

    glEnable(GL_DEPTH_TEST);
}

}

namespace Renderer {

void init(const Camera* camera, const BezierPatch* patch) {
    gCamera = camera;
    gPatch = patch;
}

void setupGL() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.12f, 0.12f, 0.15f, 1.0f);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gCamera->apply();

    drawAxes();
    drawBezierPatch();
    drawControlPoints();

    glutSwapBuffers();
}

void reshape(int width, int height) {
    if (height == 0) height = 1;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(
        45.0,
        static_cast<double>(width) / height,
        0.1,
        100.0
    );

    glMatrixMode(GL_MODELVIEW);
}

}
