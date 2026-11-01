// Probability clouds stage: Monte-Carlo point cloud of hydrogen s and p orbitals.
#include "ProbabilityCloudsStage.h"
#include <cmath>
#include <random>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Observatory palette.
const atomica::ui::Color COLOR_NUCLEUS{251, 191, 36, 255};
const atomica::ui::Color COLOR_NUCLEUS_Halo{251, 191, 36, 120};
const atomica::ui::Color COLOR_CLOUD_S{100, 140, 200, 255};   // bluish
const atomica::ui::Color COLOR_CLOUD_P{0, 229, 255, 255};     // cyan
const atomica::ui::Color COLOR_CLOUD_P2{100, 200, 220, 255};
const atomica::ui::Color COLOR_CLOUD_GRID{30, 60, 90, 255};

ProbabilityCloudsStage::ProbabilityCloudsStage(atomica::ui::ICanvas* canvas)
    : m_canvas(canvas) {
    m_nucleusRadius = 16.0f;
    m_pointDensity = 0.5f;
    m_seed = 42u;
    m_animPhase = 0.0f;
}

const char* ProbabilityCloudsStage::name() const {
    return "4. Probability Clouds";
}

void ProbabilityCloudsStage::update(double dt) {
    m_animPhase += dt * 0.8f;
}

void ProbabilityCloudsStage::render(atomica::ui::ICanvas& canvas, const atomica::ui::Rect& viewport) {
    if (!m_canvas) return;

    int cx = viewport.x + viewport.width / 2;
    int cy = viewport.y + viewport.height / 2;

    // Nucleus.
    m_canvas->fillCircle(cx, cy, static_cast<int>(m_nucleusRadius), COLOR_NUCLEUS);
    m_canvas->drawCircle(cx, cy, static_cast<int>(m_nucleusRadius) + 3, COLOR_NUCLEUS_Halo);

    // Subtle bounding grid.
    for (int k = -4; k <= 4; ++k) {
        int r = 36 + (k * 36);
        m_canvas->drawCircle(cx, cy, r, COLOR_CLOUD_GRID);
    }

    // Monte-Carlo sample |psi|^2 over a spherical shell (r = 24..66).
    std::mt19937 rng(m_seed + static_cast<unsigned>(m_animPhase * 57.0f));
    std::uniform_real_distribution<float> ang(0.0f, 2.0f * M_PI);
    std::uniform_real_distribution<float> z(-1.0f, 1.0f);
    std::uniform_real_distribution<float> rho(0.0f, 1.0f);

    int count = static_cast<int>(1800.0f * m_pointDensity);
    count = std::max(0, std::min(count, 3000));

    for (int i = 0; i < count; ++i) {
        float phi = ang(rng);
        float zv = z(rng);
        float rsq = rho(rng);
        float r = 24.0f + rsq * rsq * 42.0f; // emphasis toward the nucleus
        float theta = std::acos(zv);

        int px = cx + static_cast<int>(r * std::sin(theta) * std::cos(phi));
        int py = cy + static_cast<int>(r * std::sin(theta) * std::sin(phi));

        // Probability density colour: s (centre) vs p (angled).
        float t = (i % 3) * (1.0f / 3.0f);
        atomica::ui::Color col = COLOR_CLOUD_S;
        if (t > 0.6f) {
            col = COLOR_CLOUD_P;
        } else if (t > 0.3f) {
            col = COLOR_CLOUD_P2;
        }
        // Alpha by radial distance (denser near nucleus for s).
        float alpha = 1.0f - (r - 24.0f) / 42.0f;
        alpha = alpha * alpha;
        col.a = static_cast<uint8_t>(alpha * 255.0f);

        m_canvas->drawPixel(px, py, col);
    }

    // Axis lines for p-orbital orientation.
    m_canvas->drawLine(cx - 70, cy, cx + 70, cy, COLOR_CLOUD_GRID);
    m_canvas->drawLine(cx, cy - 70, cx, cy + 70, COLOR_CLOUD_GRID);
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
}

void ProbabilityCloudsStage::setParam(const std::string& name, float value) {
    if (name == "pointDensity") {
        m_pointDensity = value;
    }
}

void ProbabilityCloudsStage::trigger(const std::string& name) {
    (void)name;
}

void ProbabilityCloudsStage::onMouseDrag(int dx, int dy) {
    (void)dx; (void)dy;
}

void ProbabilityCloudsStage::onScroll(double delta) {
    (void)delta;
}
