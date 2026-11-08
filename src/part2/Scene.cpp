#include "Scene.h"

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <cmath>

namespace {

Vec3 hsvToRgb(float h, float s, float v) {
    float c = v * s;
    float hp = h / 60.0f;
    float x = c * (1.0f - std::fabs(std::fmod(hp, 2.0f) - 1.0f));
    Vec3 rgb;
    if (hp < 1)      rgb = {c, x, 0};
    else if (hp < 2) rgb = {x, c, 0};
    else if (hp < 3) rgb = {0, c, x};
    else if (hp < 4) rgb = {0, x, c};
    else if (hp < 5) rgb = {x, 0, c};
    else             rgb = {c, 0, x};
    float m = v - c;
    return {rgb.x + m, rgb.y + m, rgb.z + m};
}

}

Scene::Scene()
    : camera_(-90.0f, 20.0f, 11.0f),
      rng_(std::random_device{}()) {
    const float torusMajor = 0.8f;

    objects_.push_back({1, "Sphere", makeSphere(1.0f, 48, 32),
                        {-3.0f, 0.0f, 0.0f}, 0.0f,
                        {0.85f, 0.20f, 0.20f}, {0.0f, 0.0f, 0.0f}});

    objects_.push_back({2, "Rounded cube", makeRoundedCube(0.9f, 0.3f, 16),
                        {0.0f, 0.0f, 0.0f}, 0.0f,
                        {0.25f, 0.75f, 0.30f}, {0.0f, 0.0f, 0.0f}});

    objects_.push_back({3, "Torus", makeTorus(torusMajor, 0.35f, 64, 32),
                        {3.0f, 0.0f, 0.0f}, 65.0f,
                        {0.25f, 0.45f, 0.95f}, {torusMajor, 0.0f, 0.0f}});
}

SceneObject* Scene::findById(int id) {
    for (SceneObject& object : objects_) {
        if (object.id == id) {
            return &object;
        }
    }
    return nullptr;
}

Vec3 Scene::randomizeColor(SceneObject& object) {
    std::uniform_real_distribution<float> hue(0.0f, 360.0f);
    std::uniform_real_distribution<float> saturation(0.55f, 0.95f);
    std::uniform_real_distribution<float> value(0.70f, 1.0f);

    object.diffuse = hsvToRgb(hue(rng_), saturation(rng_), value(rng_));
    return object.diffuse;
}

void loadViewMatrices(const Camera& camera, int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)width / (height > 0 ? height : 1), 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    camera.apply();
}

void drawObject(const SceneObject& object, bool withNormals) {
    glPushMatrix();
    glTranslatef(object.position.x, object.position.y, object.position.z);
    glRotatef(object.tiltDeg, 1.0f, 0.0f, 0.0f);
    drawMesh(object.mesh, withNormals);
    glPopMatrix();
}
