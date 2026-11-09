// Probability clouds stage: hydrogen orbital point cloud drawn on the software canvas.
// Points come from sampleOrbital() (Orbital.cpp), coloured with the density ramp
// (stage5_shaded/DensityRamp.h). Scroll wheel cycles through OrbitalPresets.h.
#include "ProbabilityCloudsStage.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <vector>

#include "stage4_clouds/OrbitalPresets.h"
#include "stage5_shaded/DensityRamp.h"

// Observatory palette.
const atomica::ui::Color COLOR_NUCLEUS{251, 191, 36, 255};
const atomica::ui::Color COLOR_NUCLEUS_Halo{251, 191, 36, 120};
const atomica::ui::Color COLOR_CLOUD_GRID{30, 60, 90, 255};

namespace {

// 256-entry RGBA table: density (0..1) -> colour, alpha = density^2.
const std::vector<unsigned char> kRamp = ramp::buildDensityRamp(256);

// Most sampled densities are small, so lift them before the table lookup.
// If the cloud looks too faint, lower this number (for example 0.3).
constexpr float kGamma = 0.4f;

// pointDensity slider (about 0..1) -> number of electron samples.
size_t pointCountFor(float density) {
    density = std::max(0.0f, std::min(density, 2.0f));
    int count = static_cast<int>(4000.0f + 30000.0f * density);
    return static_cast<size_t>((count / 2000) * 2000);   // steps of 2000
}

}  // namespace

ProbabilityCloudsStage::ProbabilityCloudsStage(atomica::ui::ICanvas* canvas)
    : m_canvas(canvas) {
    m_nucleusRadius = 16.0f;
    m_pointDensity = 0.5f;
    m_seed = 42u;
    m_animPhase = 0.0f;
    resample();
}

const char* ProbabilityCloudsStage::name() const {
    return "4. Probability Clouds";
}

void ProbabilityCloudsStage::resample() {
    int n = 0;
    const OrbitalPreset* presets = orbitalPresets(n);
    m_preset = (m_preset % n + n) % n;
    const OrbitalPreset& p = presets[m_preset];
    m_orbital = sampleOrbital(p.n, p.l, p.m, pointCountFor(m_pointDensity), 20260102u);
    std::printf("[Stage 4] orbital %s (n=%d l=%d m=%d), %zu points\n",
                orbitalName(p.n, p.l, p.m).c_str(), p.n, p.l, p.m, m_orbital.count());
}

void ProbabilityCloudsStage::update(double dt) {
    m_animPhase += static_cast<float>(dt * 0.8);
}

void ProbabilityCloudsStage::render(atomica::ui::ICanvas& canvas, const atomica::ui::Rect& viewport) {
    (void)canvas;
    if (!m_canvas) return;

    const int cx = viewport.x + viewport.width / 2;
    const int cy = viewport.y + viewport.height / 2;
    // Stored coordinates are in units where 1.0 = the radius holding 99% of the electron.
    const float S = 0.42f * static_cast<float>(std::min(viewport.width, viewport.height));

    // Reference rings at half and full cloud radius.
    m_canvas->drawCircle(cx, cy, static_cast<int>(S * 0.5f), COLOR_CLOUD_GRID);
    m_canvas->drawCircle(cx, cy, static_cast<int>(S), COLOR_CLOUD_GRID);

    // Slow spin around the vertical axis, plus a fixed tilt so depth is visible.
    const float yaw = m_animPhase * 0.4f;
    const float cosY = std::cos(yaw), sinY = std::sin(yaw);
    const float tilt = 0.35f;
    const float cosT = std::cos(tilt), sinT = std::sin(tilt);

    const float* data = m_orbital.data.data();
    const size_t count = m_orbital.count();
    for (size_t i = 0; i < count; ++i) {
        const float* p = data + i * OrbitalCloud::kFloatsPerPoint;
        const float x = p[0], y = p[1], z = p[2], dens = p[3];

        // Spin about y, then tilt about x.
        const float x1 = x * cosY + z * sinY;
        const float z1 = -x * sinY + z * cosY;
        const float y2 = y * cosT - z1 * sinT;
        const float z2 = y * sinT + z1 * cosT;

        const int px = cx + static_cast<int>(x1 * S);
        const int py = cy - static_cast<int>(y2 * S);     // physical "up" is screen up
        if (px < viewport.x || px >= viewport.x + viewport.width ||
            py < viewport.y || py >= viewport.y + viewport.height) continue;

        // Colour + alpha from the density table.
        const float t = std::pow(std::min(std::max(dens, 0.0f), 1.0f), kGamma);
        const int idx = std::min(255, std::max(0, static_cast<int>(t * 255.0f + 0.5f)));
        const unsigned char* c = &kRamp[static_cast<size_t>(idx) * 4];

        // Points farther from the viewer fade a little (simple depth cue).
        const float shade = std::min(1.0f, std::max(0.35f, 0.75f + 0.25f * z2));
        atomica::ui::Color col{c[0], c[1], c[2], static_cast<uint8_t>(c[3] * shade)};
        m_canvas->drawPixel(px, py, col);
    }

    // Nucleus on top, small so it does not hide the centre of the cloud.
    m_canvas->fillCircle(cx, cy, 4, COLOR_NUCLEUS);
    m_canvas->drawCircle(cx, cy, 7, COLOR_NUCLEUS_Halo);
}

void ProbabilityCloudsStage::setPaused(bool paused) {
    (void)paused;
}

void ProbabilityCloudsStage::setSpeed(float speed) {
    (void)speed;
}

void ProbabilityCloudsStage::reset() {
    m_nucleusRadius = 16.0f;
    m_pointDensity = 0.5f;
    m_seed = 42u;
    m_animPhase = 0.0f;
    m_preset = 2;
    resample();
}

void ProbabilityCloudsStage::setParam(const std::string& name, float value) {
    if (name == "pointDensity") {
        m_pointDensity = value;
        if (pointCountFor(m_pointDensity) != m_orbital.count()) resample();
    }
}

void ProbabilityCloudsStage::trigger(const std::string& name) {
    (void)name;
}

void ProbabilityCloudsStage::onMouseDrag(int dx, int dy) {
    (void)dx; (void)dy;
}

void ProbabilityCloudsStage::onScroll(double delta) {
    if (delta == 0.0) return;
    m_preset += (delta > 0.0) ? 1 : -1;
    resample();
}