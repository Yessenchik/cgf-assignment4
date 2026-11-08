#include "SelfTest.h"

#include "Input.h"
#include "Renderer.h"
#include "Scene.h"

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <cstdio>
#include <cstdlib>
#include <vector>

namespace {

Scene* gScene = nullptr;
int gFailures = 0;

void check(bool ok, const char* what) {
    std::printf("[%s] %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok) gFailures++;
}

bool sameColor(Vec3 a, Vec3 b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

// Projects an object's probe point to GLUT window coordinates.
bool probeWindowPosition(const SceneObject& object, int& wx, int& wy) {
    float scale = Input::pixelScale();
    int pw = (int)(Input::windowWidth() * scale);
    int ph = (int)(Input::windowHeight() * scale);

    loadViewMatrices(gScene->camera(), pw, ph);
    glPushMatrix();
    glTranslatef(object.position.x, object.position.y, object.position.z);
    glRotatef(object.tiltDeg, 1.0f, 0.0f, 0.0f);

    GLdouble model[16], projection[16];
    GLint viewport[4] = {0, 0, pw, ph};
    glGetDoublev(GL_MODELVIEW_MATRIX, model);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glPopMatrix();

    GLdouble sx, sy, sz;
    if (!gluProject(object.probePoint.x, object.probePoint.y, object.probePoint.z,
                    model, projection, viewport, &sx, &sy, &sz)) {
        return false;
    }
    wx = (int)(sx / scale);
    wy = Input::windowHeight() - 1 - (int)(sy / scale);
    return true;
}

void testPicking(const char* label) {
    std::printf("-- picking: %s\n", label);

    for (const SceneObject& target : gScene->objects()) {
        int wx, wy;
        if (!probeWindowPosition(target, wx, wy)) {
            check(false, "project probe point");
            continue;
        }

        std::vector<Vec3> before;
        for (const SceneObject& o : gScene->objects()) before.push_back(o.diffuse);

        int id = Input::clickAt(wx, wy);

        bool othersKept = true;
        bool targetChanged = false;
        for (size_t i = 0; i < gScene->objects().size(); i++) {
            const SceneObject& o = gScene->objects()[i];
            bool same = sameColor(o.diffuse, before[i]);
            if (o.id == target.id) targetChanged = !same;
            else if (!same) othersKept = false;
        }

        char message[160];
        std::snprintf(message, sizeof(message),
                      "click (%d,%d) on %s -> id %d, only its color changed",
                      wx, wy, target.name.c_str(), id);
        check(id == target.id && targetChanged && othersKept, message);
    }

    std::vector<Vec3> before;
    for (const SceneObject& o : gScene->objects()) before.push_back(o.diffuse);
    int id = Input::clickAt(4, 4);
    bool unchanged = true;
    for (size_t i = 0; i < before.size(); i++) {
        if (!sameColor(gScene->objects()[i].diffuse, before[i])) unchanged = false;
    }
    check(id == Scene::kBackgroundId && unchanged, "background click returns 0 and changes nothing");
}

std::vector<unsigned char> captureFrame(int w, int h) {
    Renderer::drawFrame();
    std::vector<unsigned char> pixels(w * h * 4);
    glReadBuffer(GL_BACK);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    return pixels;
}

void testAntiAliasing() {
    std::printf("-- anti-aliasing\n");
    if (!Renderer::antiAliasingSupported()) {
        check(false, "MSAA supported");
        return;
    }

    float scale = Input::pixelScale();
    int w = (int)(Input::windowWidth() * scale);
    int h = (int)(Input::windowHeight() * scale);
    bool original = Renderer::antiAliasingEnabled();

    Renderer::setAntiAliasing(false);
    std::vector<unsigned char> off = captureFrame(w, h);
    Renderer::setAntiAliasing(true);
    std::vector<unsigned char> on = captureFrame(w, h);
    Renderer::setAntiAliasing(original);

    int differing = 0;
    for (int i = 0; i < w * h; i++) {
        for (int c = 0; c < 3; c++) {
            if (off[i * 4 + c] != on[i * 4 + c]) {
                differing++;
                break;
            }
        }
    }

    char message[128];
    std::snprintf(message, sizeof(message),
                  "AA on/off frames differ in %d of %d pixels (edge pixels)", differing, w * h);
    check(differing > 0 && differing < w * h / 10, message);
    check(glGetError() == GL_NO_ERROR, "no OpenGL errors");
}

void finish(int) {
    check(Input::windowWidth() == 700 && Input::windowHeight() == 520, "window resized to 700x520");
    testPicking("after resize");
    check(glGetError() == GL_NO_ERROR, "no OpenGL errors");

    std::printf("Self-test %s (%d failure%s)\n",
                gFailures == 0 ? "PASSED" : "FAILED", gFailures, gFailures == 1 ? "" : "s");
    Input::shutdown();
    std::exit(gFailures == 0 ? 0 : 1);
}

void run(int) {
    std::printf("== Part 2 self-test, pixel scale %.2f\n", Input::pixelScale());

    testPicking("initial view");

    gScene->camera().rotate(40.0f, 15.0f);
    testPicking("after camera rotation");

    gScene->camera().reset();
    testPicking("after camera reset");

    testAntiAliasing();

    glutPostRedisplay();
    glutReshapeWindow(700, 520);
    glutTimerFunc(800, finish, 0);
}

}

namespace SelfTest {

void start(Scene* scene) {
    gScene = scene;
    glutTimerFunc(800, run, 0);
}

}
