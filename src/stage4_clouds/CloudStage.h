// Stage 4 — 3D probability clouds (additive-blended points).
// Owner of rendering: R5.  Data (sampling) lives in Orbital.h/.cpp.
#pragma once
#include <string>

#include "core/Animation.h"
#include "core/Stage.h"
#include "engine3d/CloudBuffer.h"
#include "engine3d/OrbitCamera.h"

class CloudStage : public Stage {
public:
    CloudStage();
    ~CloudStage() override;

    const char* name() const override { return "Probability Clouds"; }
    void onEnter() override;
    void update(double dt) override;
    void render(int width, int height) override;
    void onKey(int key, int action) override;

    // One-line description (the UI info panel can show this).
    std::string status() const;

private:
    void showPreset(int index, bool animate);
    void resample(size_t count);

    CloudBuffer buffer_;
    OrbitCamera camera_;
    anim::Tween morph_;
    unsigned program_ = 0;
    int uMVP_ = -1, uMorph_ = -1, uSize_ = -1, uDist_ = -1, uPhase_ = -1, uAlpha_ = -1;

    int preset_ = 2;                 // start on 2p_z
    size_t count_ = 120000;
    bool phaseColors_ = false, spin_ = true;
    float spinAngle_ = 0.0f, pointSize_ = 2.2f;
    bool held_[512] = {};
    OrbitalCloud last_;              // current orbital (for status text)
};
