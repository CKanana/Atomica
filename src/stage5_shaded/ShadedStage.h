// Stage 5 — lit, depth-tested probability cloud.
// Every electron sample is drawn as a small SPHERE (a "sphere impostor": a point sprite whose
// fragment shader reconstructs the sphere's normal and depth), shaded with Blinn-Phong lighting
// from a moving light, colour fetched from a 1D texture (transfer function), plus depth fog.
// Owner: R5 (lighting) — R4 can replace the procedural texture with their own.
#pragma once
#include <string>

#include "core/Animation.h"
#include "core/Stage.h"
#include "engine3d/CloudBuffer.h"
#include "engine3d/OrbitCamera.h"

class ShadedStage : public Stage {
public:
    ShadedStage();
    ~ShadedStage() override;

    const char* name() const override { return "Shaded Rendering"; }
    void onEnter() override;
    void update(double dt) override;
    void render(int width, int height) override;
    void onKey(int key, int action) override;

    std::string status() const;

private:
    void showPreset(int index, bool animate);
    void resample(size_t count);
    void buildRampTexture();

    CloudBuffer buffer_;
    OrbitCamera camera_;
    anim::Tween morph_;
    unsigned program_ = 0, ramp_ = 0;
    int uView_ = -1, uProj_ = -1, uModel_ = -1, uMorph_ = -1, uRadius_ = -1, uPxScale_ = -1,
        uLight_ = -1, uLightI_ = -1, uPhase_ = -1, uTexOn_ = -1, uCut_ = -1, uRamp_ = -1, uMinDens_ = -1;

    int preset_ = 4;                 // start on 3d_z2
    size_t count_ = 45000;
    bool phaseColors_ = false, spin_ = true, lightOrbit_ = true, cutaway_ = false, textureOn_ = true;
    float spinAngle_ = 0.0f, lightAngle_ = 0.0f, lightIntensity_ = 1.0f, radius_ = 0.022f, minDens_ = 0.08f;
    bool held_[512] = {};
    OrbitalCloud last_;
};
