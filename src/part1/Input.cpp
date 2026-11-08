#include "Input.h"

#include "BezierPatch.h"
#include "Camera.h"

#include <GLUT/glut.h>
#include <cstdio>
#include <cstdlib>

namespace {

Camera* gCamera = nullptr;
BezierPatch* gPatch = nullptr;

constexpr float kMoveStep = 0.2f;
constexpr float kRotateStep = 5.0f;

void printSelection(const char* action) {
    int index = gPatch->selectedIndex();
    const Vec3& p = gPatch->selectedPoint();
    std::printf("%s control point %2d [%d,%d] at (%.2f, %.2f, %.2f)\n",
                action, index,
                index % BezierPatch::kSize, index / BezierPatch::kSize,
                p.x, p.y, p.z);
}

void printTessellation() {
    int r = gPatch->resolution();
    std::printf("Tessellation %dx%d (%d triangles)\n",
                r, r, gPatch->triangleCount());
}

void moveSelected(float dx, float dy, float dz) {
    gPatch->moveSelected(dx, dy, dz);
    printSelection("Moved");
}

}

namespace Input {

void init(Camera* camera, BezierPatch* patch) {
    gCamera = camera;
    gPatch = patch;
}

void printHelp() {
    std::printf(
        "Controls:\n"
        "  Arrow keys  rotate camera\n"
        "  R           reset camera\n"
        "  Tab         select next control point\n"
        "  J / L       move point -X / +X\n"
        "  I / K       move point +Y / -Y\n"
        "  U / O       move point +Z / -Z\n"
        "  + / -       increase / decrease tessellation\n"
        "  ESC         quit\n");
    printTessellation();
    printSelection("Selected");
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
        case 27: // ESC
            std::exit(0);

        case 'r':
        case 'R':
            gCamera->reset();
            std::printf("Camera reset\n");
            break;

        case 9: // TAB
            gPatch->selectNext();
            printSelection("Selected");
            break;

        case 'j': case 'J': moveSelected(-kMoveStep, 0, 0); break;
        case 'l': case 'L': moveSelected(kMoveStep, 0, 0); break;
        case 'i': case 'I': moveSelected(0, kMoveStep, 0); break;
        case 'k': case 'K': moveSelected(0, -kMoveStep, 0); break;
        case 'u': case 'U': moveSelected(0, 0, kMoveStep); break;
        case 'o': case 'O': moveSelected(0, 0, -kMoveStep); break;

        case '+':
        case '=':
            if (!gPatch->increaseResolution()) {
                std::printf("Tessellation already at maximum\n");
                return;
            }
            printTessellation();
            break;

        case '-':
        case '_':
            if (!gPatch->decreaseResolution()) {
                std::printf("Tessellation already at minimum\n");
                return;
            }
            printTessellation();
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
