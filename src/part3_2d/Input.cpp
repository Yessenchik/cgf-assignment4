#include "Input.h"

#include "Camera.h"
#include "GL3.h"
#include "Renderer.h"

#include <cstdio>
#include <cstdlib>

namespace {

Camera* gCamera = nullptr;

constexpr float kRotateStep = 5.0f;
constexpr float kZoomStep = 0.5f;

}

namespace Input {

void init(Camera* camera) {
    gCamera = camera;
}

void printHelp() {
    std::printf(
        "Controls:\n"
        "  Arrow keys  rotate camera\n"
        "  + / -       zoom in / out\n"
        "  W           toggle wireframe\n"
        "  R           reset camera\n"
        "  ESC         quit\n");
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
        case 27: // ESC
            Renderer::shutdown();
            std::exit(0);

        case 'r':
        case 'R':
            gCamera->reset();
            std::printf("Camera reset\n");
            break;

        case '+':
        case '=':
            gCamera->zoom(-kZoomStep);
            break;

        case '-':
        case '_':
            gCamera->zoom(kZoomStep);
            break;

        case 'w':
        case 'W':
            std::printf("Wireframe %s\n", Renderer::toggleWireframe() ? "ON" : "OFF");
            break;

        default:
            return;
    }

    glutPostRedisplay();
}

void specialKeys(int key, int, int) {
    switch (key) {
        case GLUT_KEY_LEFT:  gCamera->rotate(-kRotateStep, 0.0f); break;
        case GLUT_KEY_RIGHT: gCamera->rotate(kRotateStep, 0.0f); break;
        case GLUT_KEY_UP:    gCamera->rotate(0.0f, kRotateStep); break;
        case GLUT_KEY_DOWN:  gCamera->rotate(0.0f, -kRotateStep); break;
        default: return;
    }

    glutPostRedisplay();
}

}
