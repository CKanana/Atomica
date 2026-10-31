#include "stage4_clouds/CloudStage.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstdio>

#include "engine3d/Shader.h"
#include "stage4_clouds/OrbitalPresets.h"

namespace {

// Vertex shader: (1) morph A->B with a per-point delay so the cloud "melts" instead of sliding
// rigidly, (2) MVP transform, (3) point size shrinks with distance (1/w) as a depth cue,
// (4) colour from density (ramp) or phase sign.
const char* kVS = R"GLSL(
#version 330 core
layout(location = 0) in vec3 aPosA;
layout(location = 1) in vec2 aAttrA;       // x = |psi|^2 (0..1), y = phase sign
layout(location = 2) in vec3 aPosB;
layout(location = 3) in vec2 aAttrB;
uniform mat4  uMVP;
uniform float uMorph;                      // 0 = cloud A, 1 = cloud B
uniform float uSize;
uniform float uDist;
uniform int   uPhase;
out vec3 vColor;

vec3 ramp(float t) {
    vec3 c0 = vec3(0.10, 0.22, 0.95), c1 = vec3(0.70, 0.20, 0.90);
    vec3 c2 = vec3(1.00, 0.50, 0.12), c3 = vec3(1.00, 0.95, 0.75);
    if (t < 0.5)  return mix(c0, c1, t * 2.0);
    if (t < 0.85) return mix(c1, c2, (t - 0.5) / 0.35);
    return mix(c2, c3, (t - 0.85) / 0.15);
}

void main() {
    float delay = fract(sin(float(gl_VertexID) * 12.9898) * 43758.5453) * 0.4;   // 0..0.4
    float k = clamp((uMorph * 1.4 - delay) / 1.0, 0.0, 1.0);
    k = k * k * (3.0 - 2.0 * k);                                                  // smoothstep
    vec3  pos  = mix(aPosA, aPosB, k);
    float dens = mix(aAttrA.x, aAttrB.x, k);
    float sgn  = k < 0.5 ? aAttrA.y : aAttrB.y;

    gl_Position  = uMVP * vec4(pos, 1.0);
    gl_PointSize = clamp(uSize * uDist / max(gl_Position.w, 0.1), 1.0, 14.0);
    float t = pow(dens, 0.55);
    if (uPhase == 1) {
        vec3 base = sgn > 0.0 ? vec3(1.00, 0.42, 0.10) : vec3(0.15, 0.55, 1.00);
        vColor = base * (0.45 + 0.55 * t);
    } else {
        vColor = ramp(t);
    }
}
)GLSL";

const char* kFS = R"GLSL(
#version 330 core
in vec3 vColor;
uniform float uAlpha;
out vec4 fragColor;
void main() {
    vec2 d = gl_PointCoord * 2.0 - 1.0;
    float r2 = dot(d, d);
    if (r2 > 1.0) discard;                   // round points
    fragColor = vec4(vColor, uAlpha * (1.0 - r2));
}
)GLSL";

}  // namespace

CloudStage::CloudStage() {
    program_ = buildProgram(kVS, kFS);
    uMVP_ = glGetUniformLocation(program_, "uMVP");
    uMorph_ = glGetUniformLocation(program_, "uMorph");
    uSize_ = glGetUniformLocation(program_, "uSize");
    uDist_ = glGetUniformLocation(program_, "uDist");
    uPhase_ = glGetUniformLocation(program_, "uPhase");
    uAlpha_ = glGetUniformLocation(program_, "uAlpha");
    buffer_.init();
    showPreset(preset_, false);
}

CloudStage::~CloudStage() {
    buffer_.destroy();
    glDeleteProgram(program_);
}

void CloudStage::onEnter() {
    std::fill(std::begin(held_), std::end(held_), false);   // forget keys held while we were away
    std::printf("[Stage 4] %s\n", status().c_str());
    std::printf("  W/A/S/D orbit | Q/E zoom | N/B next/prev orbital | +/- points | P phase colours | Space spin | R reset\n");
}

void CloudStage::resample(size_t count) {
    count_ = count;
    int n; const OrbitalPreset* p = orbitalPresets(n);
    last_ = sampleOrbital(p[preset_].n, p[preset_].l, p[preset_].m, count_, 20260101u);
    buffer_.setImmediate(last_);
    morph_.cancel();
}

void CloudStage::showPreset(int index, bool animate) {
    int n; const OrbitalPreset* p = orbitalPresets(n);
    preset_ = (index % n + n) % n;
    if (buffer_.morphing()) { buffer_.finishMorph(); morph_.cancel(); }   // snap an unfinished morph
    last_ = sampleOrbital(p[preset_].n, p[preset_].l, p[preset_].m, count_, 20260101u);
    if (animate && buffer_.count() == last_.count() && buffer_.beginMorph(last_)) morph_.start(1.3f);
    else buffer_.setImmediate(last_);
    std::printf("[Stage 4] %s\n", status().c_str());
}

void CloudStage::update(double dt) {
    camera_.update(dt);
    if (spin_) spinAngle_ += static_cast<float>(0.35 * dt);

    const float rot = static_cast<float>(1.6 * dt);
    if (held_[GLFW_KEY_A]) camera_.orbit(-rot, 0);
    if (held_[GLFW_KEY_D]) camera_.orbit(rot, 0);
    if (held_[GLFW_KEY_W]) camera_.orbit(0, -rot);
    if (held_[GLFW_KEY_S]) camera_.orbit(0, rot);
    if (held_[GLFW_KEY_Q]) camera_.zoom(1.0f - static_cast<float>(1.2 * dt));
    if (held_[GLFW_KEY_E]) camera_.zoom(1.0f + static_cast<float>(1.2 * dt));

    if (morph_.update(dt)) buffer_.finishMorph();
}

void CloudStage::render(int w, int h) {
    glViewport(0, 0, w, h);
    glClearColor(0.01f, 0.012f, 0.025f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);                 // additive blending is order-independent

    // Model = spin about Y, View = orbit camera, Projection = perspective.
    Mat4 mvp = camera_.projection(static_cast<float>(w) / std::max(1, h)) * camera_.view() * Mat4::rotateY(spinAngle_);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_PROGRAM_POINT_SIZE);

    glUseProgram(program_);
    glUniformMatrix4fv(uMVP_, 1, GL_FALSE, mvp.m);
    glUniform1f(uMorph_, buffer_.morphing() ? morph_.progress() : 0.0f);
    glUniform1f(uSize_, pointSize_ * h / 800.0f);
    glUniform1f(uDist_, camera_.distance());
    glUniform1i(uPhase_, phaseColors_ ? 1 : 0);
    glUniform1f(uAlpha_, std::clamp(22000.0f / static_cast<float>(buffer_.count()), 0.02f, 0.5f));
    buffer_.draw();

    glDisable(GL_BLEND);                      // leave GL state clean for other stages
    glUseProgram(0);
}

void CloudStage::onKey(int key, int action) {
    if (key >= 0 && key < 512) held_[key] = (action != GLFW_RELEASE);
    if (action != GLFW_PRESS && !(action == GLFW_REPEAT && (key == GLFW_KEY_EQUAL || key == GLFW_KEY_MINUS))) return;

    switch (key) {
        case GLFW_KEY_N: showPreset(preset_ + 1, true); break;
        case GLFW_KEY_B: showPreset(preset_ - 1, true); break;
        case GLFW_KEY_EQUAL: case GLFW_KEY_KP_ADD:
            resample(std::min<size_t>(600000, static_cast<size_t>(count_ * 1.5))); break;
        case GLFW_KEY_MINUS: case GLFW_KEY_KP_SUBTRACT:
            resample(std::max<size_t>(20000, static_cast<size_t>(count_ / 1.5))); break;
        case GLFW_KEY_P: phaseColors_ = !phaseColors_; break;
        case GLFW_KEY_SPACE: spin_ = !spin_; break;
        case GLFW_KEY_LEFT_BRACKET: pointSize_ = std::max(1.0f, pointSize_ - 0.4f); break;
        case GLFW_KEY_RIGHT_BRACKET: pointSize_ = std::min(8.0f, pointSize_ + 0.4f); break;
        case GLFW_KEY_R: camera_.reset(); break;
        default: break;
    }
}

std::string CloudStage::status() const {
    char b[160];
    std::snprintf(b, sizeof b, "Orbital %s (n=%d l=%d m=%d) | %zuk points | %s colours",
                  orbitalName(last_.n, last_.l, last_.m).c_str(), last_.n, last_.l, last_.m,
                  buffer_.count() / 1000, phaseColors_ ? "phase" : "density");
    return b;
}
