#include "stage5_shaded/ShadedStage.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <vector>

#include "engine3d/Shader.h"
#include "stage4_clouds/OrbitalPresets.h"

namespace {

// Vertex shader: morph, then go to VIEW space (we light in view space), then project.
// The point sprite's size is the sphere's projected diameter: 2 * radius * pxScale / depth.
const char* kVS = R"GLSL(
#version 330 core
layout(location = 0) in vec3 aPosA;
layout(location = 1) in vec2 aAttrA;
layout(location = 2) in vec3 aPosB;
layout(location = 3) in vec2 aAttrB;
uniform mat4  uView;
uniform mat4  uModel;
uniform mat4  uProj;
uniform float uMorph;
uniform float uRadius;
uniform float uPxScale;
uniform float uMinDens;      // hide the faint outer haze so the orbital's shape stands out
out vec3  vView;       // sphere centre in view space
out float vDens;
out float vSign;
out float vModelX;     // for the cutaway plane

void main() {
    float delay = fract(sin(float(gl_VertexID) * 12.9898) * 43758.5453) * 0.4;
    float k = clamp(uMorph * 1.4 - delay, 0.0, 1.0);
    k = k * k * (3.0 - 2.0 * k);
    vec3 pos = mix(aPosA, aPosB, k);
    vDens = mix(aAttrA.x, aAttrB.x, k);
    vSign = k < 0.5 ? aAttrA.y : aAttrB.y;
    vModelX = pos.x;

    vec4 v = uView * uModel * vec4(pos, 1.0);
    vView = v.xyz;
    gl_Position = uProj * v;
    gl_PointSize = clamp(2.0 * uRadius * uPxScale / max(-v.z, 0.1), 2.0, 64.0);
    if (vDens < uMinDens) gl_Position = vec4(0.0, 0.0, 2.0, 1.0);   // outside the clip volume -> culled
}
)GLSL";

// Fragment shader: turn the flat sprite into a sphere.
//   normal  n = (x, y, sqrt(1 - x^2 - y^2))  from the position inside the sprite
//   depth   written per pixel (gl_FragDepth) so spheres intersect correctly
//   light   Blinn-Phong:  ambient + diffuse*(n.L) + specular*(n.H)^shininess
//   albedo  from a 1D texture indexed by probability density (texture mapping)
//   fog     fades distant spheres toward the background (depth shading)
const char* kFS = R"GLSL(
#version 330 core
in vec3  vView;
in float vDens;
in float vSign;
in float vModelX;
uniform mat4      uProj;
uniform float     uRadius;
uniform vec3      uLightView;      // light position in view space
uniform float     uLightI;
uniform int       uPhase;
uniform int       uTexOn;
uniform int       uCut;
uniform sampler1D uRamp;
out vec4 fragColor;

void main() {
    vec2 p = gl_PointCoord * 2.0 - 1.0;
    p.y = -p.y;                                    // gl_PointCoord origin is top-left
    float r2 = dot(p, p);
    if (r2 > 1.0) discard;                         // outside the sphere's silhouette
    if (uCut == 1 && vModelX > 0.0) discard;       // cutaway: reveal the inside

    vec3 n  = vec3(p, sqrt(1.0 - r2));             // view-space normal of the sphere
    vec3 fv = vView + n * uRadius;                 // view-space position of this pixel
    vec4 clip = uProj * vec4(fv, 1.0);
    gl_FragDepth = (clip.z / clip.w) * 0.5 + 0.5;

    vec3 albedo;
    if (uPhase == 1)      albedo = (vSign > 0.0 ? vec3(1.00, 0.45, 0.12) : vec3(0.15, 0.55, 1.00)) * (0.55 + 0.45 * vDens);
    else if (uTexOn == 1) albedo = texture(uRamp, pow(vDens, 0.4)).rgb;
    else                  albedo = vec3(0.75);

    vec3 L = normalize(uLightView - fv);
    vec3 V = normalize(-fv);
    vec3 H = normalize(L + V);
    float diff = max(dot(n, L), 0.0);
    float spec = diff > 0.0 ? pow(max(dot(n, H), 0.0), 48.0) : 0.0;

    vec3 col = albedo * (0.16 + uLightI * diff) + vec3(1.0) * spec * 0.55 * uLightI;
    float fog = smoothstep(2.2, 6.0, -fv.z);
    col = mix(col, vec3(0.01, 0.012, 0.025), fog * 0.85);
    fragColor = vec4(col, 1.0);
}
)GLSL";

}  // namespace

ShadedStage::ShadedStage() {
    program_ = buildProgram(kVS, kFS);
    uView_ = glGetUniformLocation(program_, "uView");
    uProj_ = glGetUniformLocation(program_, "uProj");
    uModel_ = glGetUniformLocation(program_, "uModel");
    uMorph_ = glGetUniformLocation(program_, "uMorph");
    uRadius_ = glGetUniformLocation(program_, "uRadius");
    uPxScale_ = glGetUniformLocation(program_, "uPxScale");
    uLight_ = glGetUniformLocation(program_, "uLightView");
    uLightI_ = glGetUniformLocation(program_, "uLightI");
    uPhase_ = glGetUniformLocation(program_, "uPhase");
    uTexOn_ = glGetUniformLocation(program_, "uTexOn");
    uCut_ = glGetUniformLocation(program_, "uCut");
    uRamp_ = glGetUniformLocation(program_, "uRamp");
    uMinDens_ = glGetUniformLocation(program_, "uMinDens");
    buffer_.init();
    buildRampTexture();
    showPreset(preset_, false);
}

ShadedStage::~ShadedStage() {
    buffer_.destroy();
    glDeleteTextures(1, &ramp_);
    glDeleteProgram(program_);
}

// 256-texel colour lookup table: probability density (0..1) -> colour. Generated in code so the
// repo needs no image assets; R4 can swap in a loaded texture later.
void ShadedStage::buildRampTexture() {
    const float stops[4][3] = {{0.10f, 0.22f, 0.95f}, {0.70f, 0.20f, 0.90f}, {1.00f, 0.50f, 0.12f}, {1.00f, 0.95f, 0.75f}};
    const float at[4] = {0.0f, 0.5f, 0.85f, 1.0f};
    std::vector<unsigned char> texels(256 * 3);
    for (int i = 0; i < 256; ++i) {
        float t = i / 255.0f;
        int s = t < at[1] ? 0 : (t < at[2] ? 1 : 2);
        float u = (t - at[s]) / (at[s + 1] - at[s]);
        for (int c = 0; c < 3; ++c)
            texels[i * 3 + c] = static_cast<unsigned char>(255.0f * anim::lerp(stops[s][c], stops[s + 1][c], u));
    }
    glGenTextures(1, &ramp_);
    glBindTexture(GL_TEXTURE_1D, ramp_);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB8, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, texels.data());
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
}

void ShadedStage::onEnter() {
    std::fill(std::begin(held_), std::end(held_), false);
    std::printf("[Stage 5] %s\n", status().c_str());
    std::printf("  W/A/S/D orbit | Q/E zoom | N/B orbital | [ ] light power | , . density cut-off | L light orbit | C cutaway | T texture on/off | P phase | Space spin\n");
}

void ShadedStage::resample(size_t count) {
    count_ = count;
    int n; const OrbitalPreset* p = orbitalPresets(n);
    last_ = sampleOrbital(p[preset_].n, p[preset_].l, p[preset_].m, count_, 20260102u);
    buffer_.setImmediate(last_);
    morph_.cancel();
}

void ShadedStage::showPreset(int index, bool animate) {
    int n; const OrbitalPreset* p = orbitalPresets(n);
    preset_ = (index % n + n) % n;
    if (buffer_.morphing()) { buffer_.finishMorph(); morph_.cancel(); }
    last_ = sampleOrbital(p[preset_].n, p[preset_].l, p[preset_].m, count_, 20260102u);
    if (animate && buffer_.count() == last_.count() && buffer_.beginMorph(last_)) morph_.start(1.3f);
    else buffer_.setImmediate(last_);
    std::printf("[Stage 5] %s\n", status().c_str());
}

void ShadedStage::update(double dt) {
    camera_.update(dt);
    if (spin_) spinAngle_ += static_cast<float>(0.3 * dt);
    if (lightOrbit_) lightAngle_ += static_cast<float>(0.9 * dt);

    const float rot = static_cast<float>(1.6 * dt);
    if (held_[GLFW_KEY_A]) camera_.orbit(-rot, 0);
    if (held_[GLFW_KEY_D]) camera_.orbit(rot, 0);
    if (held_[GLFW_KEY_W]) camera_.orbit(0, -rot);
    if (held_[GLFW_KEY_S]) camera_.orbit(0, rot);
    if (held_[GLFW_KEY_Q]) camera_.zoom(1.0f - static_cast<float>(1.2 * dt));
    if (held_[GLFW_KEY_E]) camera_.zoom(1.0f + static_cast<float>(1.2 * dt));

    if (morph_.update(dt)) buffer_.finishMorph();
}

void ShadedStage::render(int w, int h) {
    glViewport(0, 0, w, h);
    glClearColor(0.01f, 0.012f, 0.025f, 1.0f);
    glEnable(GL_DEPTH_TEST);                   // opaque spheres need real depth testing
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_PROGRAM_POINT_SIZE);

    Mat4 view = camera_.view();
    Mat4 proj = camera_.projection(static_cast<float>(w) / std::max(1, h));
    Mat4 model = Mat4::rotateY(spinAngle_);

    // Light orbits the atom in WORLD space; the shader wants it in VIEW space.
    Vec3 lightWorld{3.0f * std::cos(lightAngle_), 1.6f, 3.0f * std::sin(lightAngle_)};
    Vec3 lightView = view.transformPoint(lightWorld);

    float pxScale = h / (2.0f * std::tan(camera_.fovY * 0.5f));   // pixels per world unit at depth 1

    glUseProgram(program_);
    glUniformMatrix4fv(uView_, 1, GL_FALSE, view.m);
    glUniformMatrix4fv(uProj_, 1, GL_FALSE, proj.m);
    glUniformMatrix4fv(uModel_, 1, GL_FALSE, model.m);
    glUniform1f(uMorph_, buffer_.morphing() ? morph_.progress() : 0.0f);
    glUniform1f(uRadius_, radius_);
    glUniform1f(uMinDens_, minDens_);
    glUniform1f(uPxScale_, pxScale);
    glUniform3f(uLight_, lightView.x, lightView.y, lightView.z);
    glUniform1f(uLightI_, lightIntensity_);
    glUniform1i(uPhase_, phaseColors_ ? 1 : 0);
    glUniform1i(uTexOn_, textureOn_ ? 1 : 0);
    glUniform1i(uCut_, cutaway_ ? 1 : 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, ramp_);
    glUniform1i(uRamp_, 0);
    buffer_.draw();

    glDisable(GL_DEPTH_TEST);                  // leave GL state clean for other stages
    glUseProgram(0);
}

void ShadedStage::onKey(int key, int action) {
    if (key >= 0 && key < 512) held_[key] = (action != GLFW_RELEASE);
    bool repeatOk = action == GLFW_REPEAT && (key == GLFW_KEY_LEFT_BRACKET || key == GLFW_KEY_RIGHT_BRACKET ||
                                              key == GLFW_KEY_EQUAL || key == GLFW_KEY_MINUS ||
                                              key == GLFW_KEY_COMMA || key == GLFW_KEY_PERIOD);
    if (action != GLFW_PRESS && !repeatOk) return;

    switch (key) {
        case GLFW_KEY_N: showPreset(preset_ + 1, true); break;
        case GLFW_KEY_B: showPreset(preset_ - 1, true); break;
        case GLFW_KEY_EQUAL: case GLFW_KEY_KP_ADD:
            resample(std::min<size_t>(150000, static_cast<size_t>(count_ * 1.4))); break;
        case GLFW_KEY_MINUS: case GLFW_KEY_KP_SUBTRACT:
            resample(std::max<size_t>(8000, static_cast<size_t>(count_ / 1.4))); break;
        case GLFW_KEY_LEFT_BRACKET:  lightIntensity_ = std::max(0.0f, lightIntensity_ - 0.1f); break;
        case GLFW_KEY_RIGHT_BRACKET: lightIntensity_ = std::min(2.5f, lightIntensity_ + 0.1f); break;
        case GLFW_KEY_COMMA:  minDens_ = std::max(0.0f, minDens_ - 0.02f); break;
        case GLFW_KEY_PERIOD: minDens_ = std::min(0.9f, minDens_ + 0.02f); break;
        case GLFW_KEY_L: lightOrbit_ = !lightOrbit_; break;
        case GLFW_KEY_C: cutaway_ = !cutaway_; break;
        case GLFW_KEY_T: textureOn_ = !textureOn_; break;
        case GLFW_KEY_P: phaseColors_ = !phaseColors_; break;
        case GLFW_KEY_SPACE: spin_ = !spin_; break;
        case GLFW_KEY_R: camera_.reset(); break;
        default: break;
    }
}

std::string ShadedStage::status() const {
    char b[180];
    std::snprintf(b, sizeof b, "Orbital %s (n=%d l=%d m=%d) | %zuk spheres | light %.1f%s%s",
                  orbitalName(last_.n, last_.l, last_.m).c_str(), last_.n, last_.l, last_.m,
                  buffer_.count() / 1000, lightIntensity_, cutaway_ ? " | cutaway" : "", textureOn_ ? "" : " | flat albedo");
    return b;
}
