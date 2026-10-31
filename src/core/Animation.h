// Animation.h — small, header-only animation toolkit shared by every stage.
//   * easing functions        (smoothstep, easeInOutCubic)
//   * Tween                   (a one-shot 0..1 timer, e.g. "morph over 1.2 s")
//   * approach()              (frame-rate independent smoothing, e.g. a gliding camera)
// Everything is driven by dt (seconds since last frame), never by frame count.
#pragma once
#include <algorithm>
#include <cmath>

namespace anim {

inline float clamp01(float t) { return std::min(1.0f, std::max(0.0f, t)); }
inline float lerp(float a, float b, float t) { return a + (b - a) * t; }

// 3t^2 - 2t^3 : zero slope at both ends -> motion starts and stops gently
inline float smoothstep(float t) { t = clamp01(t); return t * t * (3.0f - 2.0f * t); }

inline float easeInOutCubic(float t) {
    t = clamp01(t);
    return t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
}

// Move `current` toward `target`; `rate` ~ how fast (1/seconds). Exponential smoothing:
// the result is the same at 30 fps and 144 fps because we use exp(-rate * dt).
inline float approach(float current, float target, float rate, double dt) {
    return current + (target - current) * (1.0f - std::exp(-rate * static_cast<float>(dt)));
}

class Tween {
public:
    void start(float seconds) { duration_ = seconds; time_ = 0.0f; active_ = true; }
    void cancel() { active_ = false; }

    // Returns true on the frame the tween finishes.
    bool update(double dt) {
        if (!active_) return false;
        time_ += static_cast<float>(dt);
        if (time_ >= duration_) { time_ = duration_; active_ = false; return true; }
        return false;
    }

    bool active() const { return active_; }
    float progress() const { return duration_ > 0.0f ? time_ / duration_ : 1.0f; }   // 0..1 linear
    float eased() const { return easeInOutCubic(progress()); }                       // 0..1 eased

private:
    float duration_ = 0.0f, time_ = 0.0f;
    bool active_ = false;
};

}  // namespace anim
