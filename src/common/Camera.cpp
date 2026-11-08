#include "Camera.h"

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <cmath>

namespace {
constexpr float PI = 3.14159265359f;
}

void Camera::reset() {
    yaw_ = kDefaultYaw;
    pitch_ = kDefaultPitch;
    distance_ = kDefaultDistance;
}

void Camera::rotate(float deltaYawDeg, float deltaPitchDeg) {
    yaw_ += deltaYawDeg;
    pitch_ += deltaPitchDeg;

    if (pitch_ > kMaxPitch) pitch_ = kMaxPitch;
    if (pitch_ < -kMaxPitch) pitch_ = -kMaxPitch;
}

Vec3 Camera::direction() const {
    float yaw = yaw_ * PI / 180.0f;
    float pitch = pitch_ * PI / 180.0f;

    return normalize({
        std::cos(pitch) * std::cos(yaw),
        std::cos(pitch) * std::sin(yaw),
        std::sin(pitch)
    });
}

void Camera::apply() const {
    float yaw = yaw_ * PI / 180.0f;
    float pitch = pitch_ * PI / 180.0f;

    float x = distance_ * std::cos(pitch) * std::cos(yaw);
    float y = distance_ * std::cos(pitch) * std::sin(yaw);
    float z = distance_ * std::sin(pitch);

    gluLookAt(
        x, y, z,
        0.0, 0.0, 0.0,
        0.0, 0.0, 1.0
    );
}
