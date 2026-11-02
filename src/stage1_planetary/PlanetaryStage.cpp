// Planet stage: electrons on fixed circular orbits.
#include "PlanetaryStage.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Observatory palette (atomica::ui::Color).
const atomica::ui::Color COLOR_NUCLEUS{251, 191, 36, 255};   // NucleusAmber
const atomica::ui::Color COLOR_NUCLEUS_Halo{251, 191, 36, 120};
const atomica::ui::Color COLOR_ORBIT{30, 60, 90, 255};       // OrbitPrimary
const atomica::ui::Color COLOR_ORBIT2{40, 80, 115, 255};     // OrbitSecondary
const atomica::ui::Color COLOR_ELECTRON{0, 229, 255, 255};   // ElectronCyan
const atomica::ui::Color COLOR_ELECTRON_GLOW{0, 229, 255, 100};

PlanetaryStage::PlanetaryStage(atomica::ui::ICanvas* canvas)
    : m_canvas(canvas) {
    m_nucleusRadius = 16.0f;
    // n=1, n=2, n=3 circular orbits.
    m_orbits.push_back({58.0f, 0.0f, 2.2f});
    m_orbits.push_back({108.0f, 1.0f, 1.6f});
    m_orbits.push_back({152.0f, 2.6f, 1.1f});
}

const char* PlanetaryStage::name() const {
    return "1. Planetary";
}

void PlanetaryStage::update(double dt) {
    for (auto& orbit : m_orbits) {
        orbit.angle += orbit.speed * dt;
        if (orbit.angle > 2.0f * M_PI) {
            orbit.angle -= 2.0f * M_PI;
        }
    }
}

void PlanetaryStage::render(atomica::ui::ICanvas& canvas, const atomica::ui::Rect& viewport) {
    if (!m_canvas) return;

    // Viewport centre.
    int cx = viewport.x + viewport.width / 2;
    int cy = viewport.y + viewport.height / 2;

    // Nucleus.
    m_canvas->fillCircle(cx, cy, static_cast<int>(m_nucleusRadius), COLOR_NUCLEUS);
    m_canvas->drawCircle(cx, cy, static_cast<int>(m_nucleusRadius) + 3, COLOR_NUCLEUS_Halo);

    // Orbits + electrons.
    for (const auto& orbit : m_orbits) {
        m_canvas->drawCircle(cx, cy, static_cast<int>(orbit.radius), COLOR_ORBIT);
        m_canvas->drawCircle(cx, cy, static_cast<int>(orbit.radius), COLOR_ORBIT2);
        int ex = cx + static_cast<int>(orbit.radius * std::cos(orbit.angle));
        int ey = cy + static_cast<int>(orbit.radius * std::sin(orbit.angle));
        m_canvas->drawCircle(ex, ey, 5, COLOR_ELECTRON);
        m_canvas->drawCircle(ex, ey, 10, COLOR_ELECTRON_GLOW);
    }
}

// R1 control hooks.
void PlanetaryStage::setPaused(bool paused) {
    // Orbital motion is time-based; the renderer gates update(dt) on paused.
    (void)paused;
}

void PlanetaryStage::setSpeed(float speed) {
    if (speed <= 0.0f) speed = 0.1f;
    for (auto& orbit : m_orbits) {
        orbit.speed = orbit.speed * (speed / 1.0f);
    }
}

void PlanetaryStage::reset() {
    m_nucleusRadius = 16.0f;
    m_orbits.clear();
    m_orbits.push_back({58.0f, 0.0f, 2.2f});
    m_orbits.push_back({108.0f, 1.0f, 1.6f});
    m_orbits.push_back({152.0f, 2.6f, 1.1f});
}

void PlanetaryStage::setParam(const std::string& name, float value) {
    if (name == "orbitRadius") {
        for (auto& orbit : m_orbits) {
            orbit.radius = 58.0f + ((orbit.radius - 58.0f) * (1.0f + (value - 0.5f) * 2.0f));
        }
    }
}

void PlanetaryStage::trigger(const std::string& name) {
    (void)name;
}

void PlanetaryStage::onMouseDrag(int dx, int dy) {
    (void)dx; (void)dy;
}

void PlanetaryStage::onScroll(double delta) {
    (void)delta;
}
