#include "StandingWavesStage.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Observatory palette
const atomica::ui::Color COLOR_NUCLEUS{251, 191, 36, 255};
const atomica::ui::Color COLOR_NUCLEUS_Halo{251, 191, 36, 120};
const atomica::ui::Color COLOR_WAVE1{0, 180, 216, 255};
const atomica::ui::Color COLOR_ORBIT{30, 60, 90, 255};
const atomica::ui::Color COLOR_NODE{0, 229, 255, 255};

StandingWavesStage::StandingWavesStage(atomica::ui::ICanvas* canvas)
    : m_canvas(canvas) {}

const char* StandingWavesStage::name() const {
    return "3. Standing Waves";
}

void StandingWavesStage::update(double dt) {
    if (!m_paused) {
        // Advance time to animate the standing wave oscillation
        m_wavePhase += dt * 3.0; 
    }
}

void StandingWavesStage::render(atomica::ui::ICanvas& canvas, const atomica::ui::Rect& viewport) {
    if (!m_canvas) return;

    int cx = viewport.x + viewport.width / 2;
    int cy = viewport.y + viewport.height / 2;

    // 1. Draw Nucleus
    m_canvas->fillCircle(cx, cy, static_cast<int>(m_nucleusRadius), COLOR_NUCLEUS);
    m_canvas->drawCircle(cx, cy, static_cast<int>(m_nucleusRadius) + 3, COLOR_NUCLEUS_Halo);

    // 2. Draw Main Orbit Base Ring
    m_canvas->drawCircle(cx, cy, static_cast<int>(m_orbitRadius), COLOR_ORBIT);

    // 3. Calculate and Draw the Bézier Standing Wave
    int n = m_currentN;
    if (n < 1) n = 1;

    // Standing wave amplitude "breathes" using cosine
    float maxAmplitude = 24.0f;
    float currentAmplitude = maxAmplitude * std::cos(m_wavePhase);

    int numLobes = 2 * n; // A wave has a crest and a trough per wavelength
    float angleStep = M_PI / n;
    float offsetAngle = M_PI / (2.0f * n); // Shift slightly so nodes are evenly spaced

    for (int i = 0; i < numLobes; ++i) {
        float thetaStart = offsetAngle + (i * angleStep);
        float thetaEnd   = offsetAngle + ((i + 1) * angleStep);

        // Alternate lobes: push outward, then inward
        float lobeAmplitude = (i % 2 == 0) ? currentAmplitude : -currentAmplitude;

        // Bézier heuristic: pull control points out 4/3 of the desired peak height
        float controlRadius = m_orbitRadius + (lobeAmplitude * 1.333f);

        // Spread the control points along the curve
        float cAngle1 = thetaStart + (angleStep / 3.0f);
        float cAngle2 = thetaEnd   - (angleStep / 3.0f);

         atomica::ui::Point2D p0 = {
         cx + static_cast<int>(m_orbitRadius * std::cos(thetaStart)),
         cy + static_cast<int>(m_orbitRadius * std::sin(thetaStart))
         };
         atomica::ui::Point2D p1 = {
         cx + static_cast<int>(controlRadius * std::cos(cAngle1)),
         cy + static_cast<int>(controlRadius * std::sin(cAngle1))
         };
         atomica::ui::Point2D p2 = {
         cx + static_cast<int>(controlRadius * std::cos(cAngle2)),
         cy + static_cast<int>(controlRadius * std::sin(cAngle2))
         };
         atomica::ui::Point2D p3 = {
         cx + static_cast<int>(m_orbitRadius * std::cos(thetaEnd)),
         cy + static_cast<int>(m_orbitRadius * std::sin(thetaEnd))
         };

        m_canvas->drawBezier(p0, p1, p2, p3, COLOR_WAVE1);

        // Draw electrons exactly at the static wave nodes
        m_canvas->fillCircle(p0.x, p0.y, 4, COLOR_NODE);
    }
}

void StandingWavesStage::setPaused(bool paused) {
    m_paused = paused;
}

void StandingWavesStage::setSpeed(float speed) {
    (void)speed;
}

void StandingWavesStage::reset() {
    m_wavePhase = 0.0f;
}

void StandingWavesStage::setParam(const std::string& name, float value) {
    // Hook up the UI Control Panel sliders!
    if (name == "waveCount" || name == "level") {
        m_currentN = static_cast<int>(value);
    } else if (name == "orbitRadius") {
        m_orbitRadius = value;
    } else if (name == "waveAmplitude") {
        // You can add an m_amplitude variable to your header later 
        // if you want to make the wave height adjustable too!
    }
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