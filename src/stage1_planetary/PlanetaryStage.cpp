#include "PlanetaryStage.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Define basic colors assuming a standard {r, g, b, a} or {r, g, b} struct in your UI namespace
const atomica::ui::Color COLOR_NUCLEUS{255, 180, 0, 255};   // Orange/Yellow
const atomica::ui::Color COLOR_ORBIT{100, 120, 140, 255};   // Faded Blue/Gray
const atomica::ui::Color COLOR_ELECTRON{0, 200, 255, 255};  // Cyan

PlanetaryStage::PlanetaryStage(atomica::ui::ICanvas* canvas) : m_canvas(canvas) {
    m_nucleusRadius = 15.0f;

    // Initialize 3 orbits: radius, starting angle, speed
    m_orbits.push_back({50.0f, 0.0f, 2.0f});   // n=1
    m_orbits.push_back({100.0f, 1.0f, 1.2f});  // n=2
    m_orbits.push_back({150.0f, 2.5f, 0.8f});  // n=3
}

const char* PlanetaryStage::name() const {
    return "1. Planetary";
}

void PlanetaryStage::update(double dt) {
    // Advance the angle of each electron based on its speed and the delta time
    for (auto& orbit : m_orbits) {
        orbit.angle += orbit.speed * dt;
        
        if (orbit.angle > 2 * M_PI) {
            orbit.angle -= 2 * M_PI;
        }
    }
}

void PlanetaryStage::render(int width, int height) {
    if (!m_canvas) return;

    int cx = width / 2;
    int cy = height / 2;

    // 1. Draw Nucleus
    m_canvas->fillCircle(cx, cy, static_cast<int>(m_nucleusRadius), COLOR_NUCLEUS);

    // 2. Draw Orbits and Electrons
    for (const auto& orbit : m_orbits) {
        // Draw the hollow orbit path
        m_canvas->drawCircle(cx, cy, static_cast<int>(orbit.radius), COLOR_ORBIT);

        // Calculate electron Cartesian coordinates from polar coordinates
        int ex = cx + static_cast<int>(orbit.radius * std::cos(orbit.angle));
        int ey = cy + static_cast<int>(orbit.radius * std::sin(orbit.angle));

        // Draw the electron
        m_canvas->fillCircle(ex, ey, 5, COLOR_ELECTRON);
    }
}