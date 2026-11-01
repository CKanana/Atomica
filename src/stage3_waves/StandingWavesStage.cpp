// Standing waves stage: matter waves around the nucleus (Bézier splines).
#include "StandingWavesStage.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Observatory palette.
const atomica::ui::Color COLOR_NUCLEUS{251, 191, 36, 255};
const atomica::ui::Color COLOR_NUCLEUS_Halo{251, 191, 36, 120};
const atomica::ui::Color COLOR_WAVE1{0, 180, 216, 255};      // WaveLine
const atomica::ui::Color COLOR_WAVE2{0, 140, 170, 255};
const atomica::ui::Color COLOR_WAVE3{100, 200, 220, 255};
const atomica::ui::Color COLOR_WAVE4{0, 180, 216, 255};
const atomica::ui::Color COLOR_ORBIT{30, 60, 90, 255};       // OrbitPrimary
const atomica::ui::Color COLOR_ORBIT2{40, 80, 115, 255};     // OrbitSecondary
const atomica::ui::Color COLOR_ELECTRON{0, 229, 255, 255};

StandingWavesStage::StandingWavesStage(atomica::ui::ICanvas* canvas)
    : m_canvas(canvas) {
    m_nucleusRadius = 16.0f;
    m_wavePhase = 0.0f;
    // Standing-wave nodes on the main orbit.
    for (int n = 1; n <= 4; ++n) {
        m_nodes.push_back({38.0f + (n * 22.0f), 0.0f, static_cast<float>(n)});
    }
}

const char* StandingWavesStage::name() const {
    return "3. Standing Waves";
}

void StandingWavesStage::update(double dt) {
    m_wavePhase += dt;
}

void StandingWavesStage::render(atomica::ui::ICanvas& canvas, const atomica::ui::Rect& viewport) {
    if (!m_canvas) return;

    int cx = viewport.x + viewport.width / 2;
    int cy = viewport.y + viewport.height / 2;

    // Nucleus.
    m_canvas->fillCircle(cx, cy, static_cast<int>(m_nucleusRadius), COLOR_NUCLEUS);
    m_canvas->drawCircle(cx, cy, static_cast<int>(m_nucleusRadius) + 3, COLOR_NUCLEUS_Halo);

    // Main orbit ring.
    m_canvas->drawCircle(cx, cy, 64, COLOR_ORBIT);
    m_canvas->drawCircle(cx, cy, 64, COLOR_ORBIT2);

    // Standing-wave loops (Bézier splines) onto the main orbit.
    // 2-lobed, 3-lobed, 4-lobed matter-wave shapes around the ring.
    for (const auto& node : m_nodes) {
        float n = node.n;
        float R = 64.0f;
        float phase = m_wavePhase * (n * 0.8f);
        // Parametric: r(theta) = R + A * cos(n*theta)
        int numSegs = 40;
        int prevX = cx + static_cast<int>((R) * std::cos(0.0f));
        int prevY = cy + static_cast<int>((R) * std::sin(0.0f));
        for (int i = 1; i <= numSegs; ++i) {
            float t = static_cast<float>(i) / numSegs;
            float theta = t * 2.0f * M_PI;
            float r = R + 16.0f * std::cos(n * theta + phase);
            float x = cx + static_cast<int>(r * std::cos(theta));
            float y = cy + static_cast<int>(r * std::sin(theta));
            m_canvas->drawLine(prevX, prevY, x, y, COLOR_WAVE1);
            prevX = x; prevY = y;
        }
        // Node dots.
        for (int k = 0; k < 3; ++k) {
            float a = k * (2.0f * M_PI / 3.0f);
            int nx = cx + static_cast<int>(R * std::cos(a));
            int ny = cy + static_cast<int>(R * std::sin(a));
            m_canvas->fillCircle(nx, ny, 5, COLOR_ELECTRON);
        }
    }

    // Central wave-lobe (de Broglie) preview.
    m_canvas->drawBezier(
        {cx - 24, cy - 10}, {cx - 4, cy - 34}, {cx + 16, cy - 8}, {cx + 14, cy - 10}, COLOR_WAVE2);
    m_canvas->drawBezier(
        {cx + 24, cy + 10}, {cx + 4, cy + 34}, {cx - 16, cy + 8}, {cx - 14, cy + 10}, COLOR_WAVE2);
}

void StandingWavesStage::setPaused(bool paused) {
    (void)paused;
}

void StandingWavesStage::setSpeed(float speed) {
    (void)speed;
}

void StandingWavesStage::reset() {
    m_nucleusRadius = 16.0f;
    m_wavePhase = 0.0f;
    for (auto& node : m_nodes) {
        node.angle = 0.0f;
        node.n = 0;
    }
}

void StandingWavesStage::setParam(const std::string& name, float value) {
    (void)name; (void)value;
}

void StandingWavesStage::trigger(const std::string& name) {
    (void)name;
}

void StandingWavesStage::onMouseDrag(int dx, int dy) {
    (void)dx; (void)dy;
}

void StandingWavesStage::onScroll(double delta) {
    (void)delta;
}
