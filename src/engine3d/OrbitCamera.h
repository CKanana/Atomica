// OrbitCamera.h — a camera that circles the origin (where the nucleus is).
// Input changes the TARGET yaw/pitch/distance; update() glides the real values toward
// them, so keyboard control feels smooth instead of jumpy.
#pragma once
#include <algorithm>

#include "core/Animation.h"
#include "engine3d/Mat4.h"

class OrbitCamera {
public:
    float fovY = 0.9f, zNear = 0.1f, zFar = 50.0f;

    void orbit(float dYaw, float dPitch) {
        yawT_ += dYaw;
        pitchT_ = std::clamp(pitchT_ + dPitch, -1.5f, 1.5f);
    }
    void zoom(float factor) { distT_ = std::clamp(distT_ * factor, 1.3f, 9.0f); }
    void reset() { yawT_ = 0.0f; pitchT_ = 0.35f; distT_ = 2.6f; }

    void update(double dt) {
        yaw_ = anim::approach(yaw_, yawT_, 10.0f, dt);
        pitch_ = anim::approach(pitch_, pitchT_, 10.0f, dt);
        dist_ = anim::approach(dist_, distT_, 8.0f, dt);
    }

    // View = translate(0,0,-d) * Rx(pitch) * Ry(yaw): rotate the world, then push it away.
    Mat4 view() const { return Mat4::translate(0, 0, -dist_) * Mat4::rotateX(pitch_) * Mat4::rotateY(yaw_); }
    Mat4 projection(float aspect) const { return Mat4::perspective(fovY, aspect, zNear, zFar); }
    float distance() const { return dist_; }

private:
    float yaw_ = 0.0f, pitch_ = 0.35f, dist_ = 2.6f;
    float yawT_ = 0.0f, pitchT_ = 0.35f, distT_ = 2.6f;
};
