#pragma once

#include "Vec3.h"

class Camera {
public:
    static constexpr float kDefaultYaw = 45.0f;
    static constexpr float kDefaultPitch = 35.0f;
    static constexpr float kDefaultDistance = 10.0f;
    static constexpr float kMaxPitch = 85.0f;

    void reset();

    void rotate(float deltaYawDeg, float deltaPitchDeg);

    Vec3 direction() const;

    void apply() const;

    float yaw() const { return yaw_; }
    float pitch() const { return pitch_; }
    float distance() const { return distance_; }

private:
    float yaw_ = kDefaultYaw;
    float pitch_ = kDefaultPitch;
    float distance_ = kDefaultDistance;
};
