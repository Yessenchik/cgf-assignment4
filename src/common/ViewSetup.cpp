#include "ViewSetup.h"

ViewSetup computeView(const Camera& camera, int width, int height) {
    ViewSetup setup;
    setup.eye = camera.position();

    const Vec3 target = {0.0f, 0.0f, 0.0f};
    const Vec3 worldUp = {0.0f, 0.0f, 1.0f};
    setup.view = lookAt(setup.eye, target, worldUp);
    setup.projection = perspective(45.0f, (float)width / (height > 0 ? height : 1), 0.1f, 100.0f);

    Vec3 forward = normalize(subtract(target, setup.eye));
    Vec3 right = normalize(cross(forward, worldUp));
    Vec3 up = cross(right, forward);
    float offset = 0.3f * camera.distance();
    setup.light = add(setup.eye, add(scale(right, offset), scale(up, offset)));
    return setup;
}
