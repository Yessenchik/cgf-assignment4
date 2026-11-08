#pragma once

#include "Camera.h"
#include "Mat4.h"

struct ViewSetup {
    Mat4 view;
    Mat4 projection;
    Vec3 eye;
    Vec3 light;
};

ViewSetup computeView(const Camera& camera, int width, int height);
