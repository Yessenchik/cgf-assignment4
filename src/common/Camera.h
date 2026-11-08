#pragma once

#include "Vec3.h"

// Orbit camera looking at the origin with Z as the up axis.
class Camera {
public:
    static constexpr float kDefaultYaw = 45.0f;
    static constexpr float kDefaultPitch = 35.0f;
    static constexpr float kDefaultDistance = 10.0f;
    static constexpr float kMaxPitch = 85.0f;

    void reset();

    // Rotate by the given angles in degrees; pitch is clamped to avoid flipping.
    void rotate(float deltaYawDeg, float deltaPitchDeg);

    // Unit vector from the origin towards the camera (used as the light direction).
    Vec3 direction() const;

    // Loads the view transform with gluLookAt (assumes GL_MODELVIEW is current).
    void apply() const;

    float yaw() const { return yaw_; }
    float pitch() const { return pitch_; }
    float distance() const { return distance_; }

private:
    float yaw_ = kDefaultYaw;
    float pitch_ = kDefaultPitch;
    float distance_ = kDefaultDistance;
};
