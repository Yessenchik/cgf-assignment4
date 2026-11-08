#include "Camera.h"

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <cmath>

namespace {
constexpr float PI = 3.14159265359f;
}

Camera::Camera(float yaw, float pitch, float distance)
    : initialYaw_(yaw),
      initialPitch_(pitch),
      initialDistance_(distance),
      yaw_(yaw),
      pitch_(pitch),
      distance_(distance) {}

void Camera::reset() {
    yaw_ = initialYaw_;
    pitch_ = initialPitch_;
    distance_ = initialDistance_;
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

Vec3 Camera::position() const {
    float yaw = yaw_ * PI / 180.0f;
    float pitch = pitch_ * PI / 180.0f;

    return {
        distance_ * std::cos(pitch) * std::cos(yaw),
        distance_ * std::cos(pitch) * std::sin(yaw),
        distance_ * std::sin(pitch)
    };
}

void Camera::apply() const {
    Vec3 eye = position();

    gluLookAt(
        eye.x, eye.y, eye.z,
        0.0, 0.0, 0.0,
        0.0, 0.0, 1.0
    );
}
