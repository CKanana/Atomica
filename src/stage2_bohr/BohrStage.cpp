// Bohr stage: quantised energy levels and photon transitions.
#include "BohrStage.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Observatory palette.
const atomica::ui::Color COLOR_NUCLEUS{251, 191, 36, 255};
const atomica::ui::Color COLOR_NUCLEUS_Halo{251, 191, 36, 120};
const atomica::ui::Color COLOR_ORBIT{30, 60, 90, 255};
const atomica::ui::Color COLOR_ORBIT2{40, 80, 115, 255};
const atomica::ui::Color COLOR_ELECTRON{0, 229, 255, 255};
const atomica::ui::Color COLOR_ELECTRON_GLOW{0, 229, 255, 100};
const atomica::ui::Color COLOR_PHOTON{0, 229, 255, 255};
const atomica::ui::Color COLOR_LEVEL_LINE{100, 116, 139, 255};

BohrStage::BohrStage(atomica::ui::ICanvas* canvas)
    : m_canvas(canvas) {
    m_nucleusRadius = 16.0f;
    m_level = 1;
    m_transitioning = false;
    m_photonPhase = 0.0f;
    // Energy levels: n=1,2,3,4.
    m_levels.resize(4);
    for (int n = 0; n < 4; ++n) {
        m_levels[n].radius = 40.0f + (n * 55.0f);
        m_levels[n].angle = 0.0f;
    }
}

const char* BohrStage::name() const {
    return "2. Bohr";
}

void BohrStage::update(double dt) {
    if (m_transitioning) {
        // Slide the electron between levels over a short window.
        m_photonPhase += dt * 4.0f;
        if (m_photonPhase > 1.0f) {
            m_photonPhase = 1.0f;
        }
        // Advance orbiting of the current level.
        for (int n = 0; n < 4; ++n) {
            if (n + 1 == m_level) {
                m_levels[n].angle += 2.5f * dt;
            }
        }
        return;
    }
    // Orbital motion at the current level.
    if (m_level >= 1 && m_level <= 4) {
        int n = m_level - 1;
        m_levels[n].angle += 2.5f * dt;
    }
}

void BohrStage::render(atomica::ui::ICanvas& canvas, const atomica::ui::Rect& viewport) {
    if (!m_canvas) return;

    int cx = viewport.x + viewport.width / 2;
    int cy = viewport.y + viewport.height / 2;

    // Nucleus.
    m_canvas->fillCircle(cx, cy, static_cast<int>(m_nucleusRadius), COLOR_NUCLEUS);
    m_canvas->drawCircle(cx, cy, static_cast<int>(m_nucleusRadius) + 3, COLOR_NUCLEUS_Halo);

    // Energy level circles + labels.
    for (int n = 0; n < 4; ++n) {
        float r = m_levels[n].radius;
        // Level line (radial-ish ring).
        m_canvas->drawCircle(cx, cy, static_cast<int>(r), COLOR_ORBIT);
        m_canvas->drawCircle(cx, cy, static_cast<int>(r), COLOR_ORBIT2);
        // Electron on this level.
        if (n + 1 == m_level) {
            float ex = cx + static_cast<int>(r * std::cos(m_levels[n].angle));
            float ey = cy + static_cast<int>(r * std::sin(m_levels[n].angle));
            m_canvas->drawCircle(static_cast<int>(ex), static_cast<int>(ey), 6, COLOR_ELECTRON);
            m_canvas->drawCircle(static_cast<int>(ex), static_cast<int>(ey), 11, COLOR_ELECTRON_GLOW);
            // Level label (n).
            m_canvas->drawText(
                cx + static_cast<int>(r) + 14, static_cast<int>(cy) - 10, std::to_string(n + 1), COLOR_PHOTON);
        }
        // Level label at bottom.
        m_canvas->drawText(
            cx + 20, cy + static_cast<int>(r) + 18, "n=" + std::to_string(n + 1), COLOR_LEVEL_LINE);
    }

    // Photon transition indicator when jumping.
    if (m_transitioning) {
        float t = m_photonPhase;
        float srcR = m_levels[m_level - 2].radius;
        float dstR = m_levels[m_level - 1].radius;
        float tx = cx + static_cast<int>(srcR * std::cos(m_levels[m_level - 2].angle));
        float ty = cy + static_cast<int>(srcR * std::sin(m_levels[m_level - 2].angle));
        float ex = cx + static_cast<int>(dstR * std::cos(m_levels[m_level - 1].angle));
        float ey = cy + static_cast<int>(dstR * std::sin(m_levels[m_level - 1].angle));
        // Fade the photon in along the path.
        int px = static_cast<int>(tx + (ex - tx) * t);
        int py = static_cast<int>(ty + (ey - ty) * t);
        int srcR2 = static_cast<int>(srcR);
        int dstR2 = static_cast<int>(dstR);
        m_canvas->drawLine(px, py, px, py, COLOR_PHOTON);
        if (t < 1.0f) {
            m_canvas->drawCircle(px, py, 6, COLOR_PHOTON);
            m_canvas->drawCircle(px, py, 12, COLOR_PHOTON);
        } else {
            // Snap the electron to the new level.
            m_level = m_level;
        }
    }
}

void BohrStage::setPaused(bool paused) {
    (void)paused;
}

void BohrStage::setSpeed(float speed) {
    (void)speed;
}

void BohrStage::reset() {
    m_nucleusRadius = 16.0f;
    m_level = 1;
    m_transitioning = false;
    m_photonPhase = 0.0f;
    for (int n = 0; n < 4; ++n) {
        m_levels[n].radius = 40.0f + (n * 55.0f);
        m_levels[n].angle = 0.0f;
    }
}

void BohrStage::setParam(const std::string& name, float value) {
    if (name == "level") {
        m_level = static_cast<int>(value);
        if (m_level < 1) m_level = 1;
        if (m_level > 4) m_level = 4;
    }
}

void BohrStage::trigger(const std::string& name) {
    if (name == "photonJump") {
        m_transitioning = true;
        m_photonPhase = 0.0f;
        if (m_level < 4) {
            m_level++;
        } else {
            m_level = 1;
        }
    }
}

void BohrStage::onMouseDrag(int dx, int dy) {
    (void)dx; (void)dy;
}

void BohrStage::onScroll(double delta) {
    (void)delta;
}
