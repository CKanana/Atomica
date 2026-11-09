#include "PlanetaryStage.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const atomica::ui::Color COLOR_NUCLEUS{251, 191, 36, 255};
const atomica::ui::Color COLOR_NUCLEUS_Halo{251, 191, 36, 120};
const atomica::ui::Color COLOR_ORBIT{30, 60, 90, 255};
const atomica::ui::Color COLOR_ELECTRON{0, 229, 255, 255};
const atomica::ui::Color COLOR_ELECTRON_GLOW{0, 229, 255, 100};

PlanetaryStage::PlanetaryStage(atomica::ui::ICanvas* canvas)
    : m_canvas(canvas) {
    reset();
}

const char* PlanetaryStage::name() const {
    return "1. Planetary";
}

void PlanetaryStage::update(double dt) {
    if (m_paused) return;

    for (auto& orbit : m_orbits) {
        orbit.angle += orbit.baseSpeed * m_speedMultiplier * dt;
        if (orbit.angle > 2.0f * M_PI) {
            orbit.angle -= 2.0f * M_PI;
        }
    }
}

void PlanetaryStage::render(atomica::ui::ICanvas& canvas, const atomica::ui::Rect& viewport) {
    if (!m_canvas) return;

    int cx = viewport.x + viewport.width / 2;
    int cy = viewport.y + viewport.height / 2;

    // Draw Nucleus
    m_canvas->fillCircle(cx, cy, static_cast<int>(m_nucleusRadius), COLOR_NUCLEUS);
    m_canvas->drawCircle(cx, cy, static_cast<int>(m_nucleusRadius) + 3, COLOR_NUCLEUS_Halo);

    // Draw Elliptical Orbits + Electrons
    for (const auto& orbit : m_orbits) {
        // Classical mechanics parameters for an ellipse
        float a = orbit.baseRadius * m_radiusScale;      // Semi-major axis
        float e = m_eccentricity;                        // Eccentricity
        float b = a * std::sqrt(1.0f - e * e);           // Semi-minor axis
        float c = a * e;                                 // Distance from center to focus

        // Offset the ellipse so the nucleus is positioned at the right focal point
        float ellipseCenterX = cx - c;
        float ellipseCenterY = cy;

        // Draw the continuous elliptical path using 60 line segments
        int numSegments = 60;
        int prevX = static_cast<int>(ellipseCenterX + a * std::cos(0.0f));
        int prevY = static_cast<int>(ellipseCenterY + b * std::sin(0.0f));

        for (int i = 1; i <= numSegments; ++i) {
            float theta = (static_cast<float>(i) / numSegments) * 2.0f * M_PI;
            int x = static_cast<int>(ellipseCenterX + a * std::cos(theta));
            int y = static_cast<int>(ellipseCenterY + b * std::sin(theta));
            m_canvas->drawLine(prevX, prevY, x, y, COLOR_ORBIT);
            prevX = x; 
            prevY = y;
        }

        // Calculate current electron position on the ellipse
        int ex = static_cast<int>(ellipseCenterX + a * std::cos(orbit.angle));
        int ey = static_cast<int>(ellipseCenterY + b * std::sin(orbit.angle));

        m_canvas->fillCircle(ex, ey, 5, COLOR_ELECTRON);
        m_canvas->drawCircle(ex, ey, 10, COLOR_ELECTRON_GLOW);
    }
}

void PlanetaryStage::setPaused(bool paused) {
    m_paused = paused;
}

void PlanetaryStage::setSpeed(float speed) {
    m_speedMultiplier = speed;
}

void PlanetaryStage::reset() {
    m_orbits.clear();
    m_orbits.push_back({58.0f, 0.0f, 2.2f});
    m_orbits.push_back({108.0f, 1.0f, 1.6f});
    m_orbits.push_back({152.0f, 2.6f, 1.1f});
    
    m_radiusScale = 1.0f;
    m_eccentricity = 0.0f;
    m_speedMultiplier = 1.0f;
}

void PlanetaryStage::setParam(const std::string& name, float value) {
    if (name == "orbitRadius") {
        // The default UI slider starts at 120. We divide by 120 to calculate the scaling multiplier.
        m_radiusScale = value / 120.0f; 
    } else if (name == "eccentricity") {
        // Limit eccentricity to 0.95 to prevent the ellipse from flattening into a straight line or crashing the sqrt()
        m_eccentricity = std::max(0.0f, std::min(value, 0.95f));
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