#include "Input.h"

#include "BezierPatch.h"
#include "Camera.h"

#include <GLUT/glut.h>
#include <cstdlib>

namespace {

Camera* gCamera = nullptr;
BezierPatch* gPatch = nullptr;

constexpr float kMoveStep = 0.2f;
constexpr float kRotateStep = 5.0f;

}

namespace Input {

void init(Camera* camera, BezierPatch* patch) {
    gCamera = camera;
    gPatch = patch;
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
        case 27: // ESC
            std::exit(0);
            break;

        case 'r':
        case 'R':
            gCamera->reset();
            break;

        case 9: // TAB
            gPatch->selectNext();
            break;

        case 'j':
        case 'J':
            gPatch->moveSelected(-kMoveStep, 0, 0);
            break;

        case 'l':
        case 'L':
            gPatch->moveSelected(kMoveStep, 0, 0);
            break;

        case 'i':
        case 'I':
            gPatch->moveSelected(0, kMoveStep, 0);
            break;

        case 'k':
        case 'K':
            gPatch->moveSelected(0, -kMoveStep, 0);
            break;

        case 'u':
        case 'U':
            gPatch->moveSelected(0, 0, kMoveStep);
            break;

        case 'o':
        case 'O':
            gPatch->moveSelected(0, 0, -kMoveStep);
            break;

        case '+':
        case '=':
            gPatch->increaseResolution();
            break;

        case '-':
        case '_':
            gPatch->decreaseResolution();
            break;
    }

    glutPostRedisplay();
}

void specialKeys(int key, int, int) {
    switch (key) {
        case GLUT_KEY_LEFT:
            gCamera->rotate(-kRotateStep, 0.0f);
            break;

        case GLUT_KEY_RIGHT:
            gCamera->rotate(kRotateStep, 0.0f);
            break;

        case GLUT_KEY_UP:
            gCamera->rotate(0.0f, kRotateStep);
            break;

        case GLUT_KEY_DOWN:
            gCamera->rotate(0.0f, -kRotateStep);
            break;
    }

    glutPostRedisplay();
}

}
