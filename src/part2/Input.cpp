#include "Input.h"

#include "Picking.h"
#include "Renderer.h"
#include "Scene.h"

#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace {

Scene* gScene = nullptr;
PickingBuffer* gPicker = nullptr;

int gWindowWidth = 1;
int gWindowHeight = 1;
float gPixelScale = 0.0f;

constexpr float kRotateStep = 5.0f;

int pixelWidth() { return (int)std::lround(gWindowWidth * gPixelScale); }
int pixelHeight() { return (int)std::lround(gWindowHeight * gPixelScale); }

}

namespace Input {

void init(Scene* scene, PickingBuffer* picker) {
    gScene = scene;
    gPicker = picker;
}

void printHelp() {
    std::printf(
        "Controls:\n"
        "  Arrow keys   rotate camera\n"
        "  R            reset camera\n"
        "  A            toggle anti-aliasing\n"
        "  Left click   pick object and give it a random color\n"
        "  ESC          quit\n");
}

void shutdown() {
    gPicker->destroy();
    Renderer::shutdown();
}

int windowWidth() { return gWindowWidth; }
int windowHeight() { return gWindowHeight; }
float pixelScale() { return gPixelScale; }

void reshape(int width, int height) {
    gWindowWidth = std::max(1, width);
    gWindowHeight = std::max(1, height);

    // The initial viewport equals the drawable size, which reveals Retina scaling.
    if (gPixelScale <= 0.0f) {
        GLint viewport[4] = {0, 0, 0, 0};
        glGetIntegerv(GL_VIEWPORT, viewport);
        gPixelScale = viewport[2] > 0 ? (float)viewport[2] / gWindowWidth : 1.0f;
    }

    Renderer::resize(pixelWidth(), pixelHeight());
    if (!gPicker->resize(pixelWidth(), pixelHeight())) {
        std::fprintf(stderr, "Picking disabled: framebuffer unavailable\n");
    }

    std::printf("Window %dx%d (framebuffer %dx%d)\n",
                gWindowWidth, gWindowHeight, pixelWidth(), pixelHeight());
}

int clickAt(int windowX, int windowY) {
    int px = (int)((windowX + 0.5f) * gPixelScale);
    int py = (int)((gWindowHeight - windowY - 0.5f) * gPixelScale);
    px = std::clamp(px, 0, pixelWidth() - 1);
    py = std::clamp(py, 0, pixelHeight() - 1);

    int id = gPicker->pick(*gScene, px, py);

    if (id < 0) {
        std::printf("Picking unavailable\n");
        return id;
    }
    if (id == Scene::kBackgroundId) {
        std::printf("Clicked background\n");
        return id;
    }

    SceneObject* object = gScene->findById(id);
    if (!object) {
        std::printf("Unknown object id %d\n", id);
        return id;
    }

    Vec3 c = gScene->randomizeColor(*object);
    std::printf("Picked %s (id %d), new color (%.2f, %.2f, %.2f)\n",
                object->name.c_str(), id, c.x, c.y, c.z);

    glutPostRedisplay();
    return id;
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
        case 27: // ESC
            shutdown();
            std::exit(0);

        case 'r':
        case 'R':
            gScene->camera().reset();
            std::printf("Camera reset\n");
            break;

        case 'a':
        case 'A':
            if (!Renderer::antiAliasingSupported()) {
                std::printf("Anti-aliasing not supported on this system\n");
                return;
            }
            Renderer::setAntiAliasing(!Renderer::antiAliasingEnabled());
            std::printf("Anti-aliasing %s (%dx MSAA)\n",
                        Renderer::antiAliasingEnabled() ? "ON" : "OFF",
                        Renderer::sampleCount());
            break;

        default:
            return;
    }

    glutPostRedisplay();
}

void specialKeys(int key, int, int) {
    Camera& camera = gScene->camera();

    switch (key) {
        case GLUT_KEY_LEFT:  camera.rotate(-kRotateStep, 0.0f); break;
        case GLUT_KEY_RIGHT: camera.rotate(kRotateStep, 0.0f); break;
        case GLUT_KEY_UP:    camera.rotate(0.0f, kRotateStep); break;
        case GLUT_KEY_DOWN:  camera.rotate(0.0f, -kRotateStep); break;
        default: return;
    }

    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        clickAt(x, y);
    }
}

}
