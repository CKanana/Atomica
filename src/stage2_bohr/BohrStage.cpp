#include "BohrStage.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const atomica::ui::Color COLOR_NUCLEUS{251, 191, 36, 255};
const atomica::ui::Color COLOR_NUCLEUS_Halo{251, 191, 36, 120};
const atomica::ui::Color COLOR_ORBIT{30, 60, 90, 255};
const atomica::ui::Color COLOR_ORBIT2{40, 80, 115, 255};
const atomica::ui::Color COLOR_ELECTRON{0, 229, 255, 255};
const atomica::ui::Color COLOR_ELECTRON_GLOW{0, 229, 255, 100};
const atomica::ui::Color COLOR_PHOTON{255, 100, 200, 255}; // Distinct color for light
const atomica::ui::Color COLOR_LEVEL_LINE{100, 116, 139, 255};

BohrStage::BohrStage(atomica::ui::ICanvas* canvas)
    : m_canvas(canvas) {
    reset();
}

const char* BohrStage::name() const {
    return "2. Bohr";
}

void BohrStage::update(double dt) {
    if (m_paused) return;

    // Advance all discrete orbits based on their quantized velocities
    for (int i = 0; i < 4; ++i) {
        m_levels[i].angle += m_levels[i].speed * m_speedMultiplier * dt;
        if (m_levels[i].angle > 2.0f * M_PI) {
            m_levels[i].angle -= 2.0f * M_PI;
        }
    }

    // Animate the photon jump
    if (m_transitioning) {
        m_transitionProgress += dt * 2.0f; // Half-second transition speed
        if (m_transitionProgress >= 1.0f) {
            m_transitionProgress = 1.0f;
            m_currentLevel = m_targetLevel;
            m_transitioning = false;
        }
    }
}

void BohrStage::render(atomica::ui::ICanvas& canvas, const atomica::ui::Rect& viewport) {
    if (!m_canvas) return;

    int cx = viewport.x + viewport.width / 2;
    int cy = viewport.y + viewport.height / 2;

    // 1. Draw Nucleus
    m_canvas->fillCircle(cx, cy, static_cast<int>(m_nucleusRadius), COLOR_NUCLEUS);
    m_canvas->drawCircle(cx, cy, static_cast<int>(m_nucleusRadius) + 3, COLOR_NUCLEUS_Halo);

    // 2. Draw Quantized Energy Levels
    for (int n = 0; n < 4; ++n) {
        float r = m_levels[n].radius;
        m_canvas->drawCircle(cx, cy, static_cast<int>(r), COLOR_ORBIT);
        m_canvas->drawCircle(cx, cy, static_cast<int>(r), COLOR_ORBIT2);
        m_canvas->drawText(cx + 20, cy + static_cast<int>(r) + 18, "n=" + std::to_string(n + 1), COLOR_LEVEL_LINE);
    }

    // 3. Draw Electron and Photon
    if (!m_transitioning) {
        // Stable state: electron sits strictly on its allowed orbital
        float r = m_levels[m_currentLevel - 1].radius;
        float a = m_levels[m_currentLevel - 1].angle;
        int ex = cx + static_cast<int>(r * std::cos(a));
        int ey = cy + static_cast<int>(r * std::sin(a));
        
        m_canvas->fillCircle(ex, ey, 5, COLOR_ELECTRON);
        m_canvas->drawCircle(ex, ey, 10, COLOR_ELECTRON_GLOW);
    } else {
        // Transitioning state: smoothstep interpolation for visual leap
        float r1 = m_levels[m_sourceLevel - 1].radius;
        float a1 = m_levels[m_sourceLevel - 1].angle;
        float r2 = m_levels[m_targetLevel - 1].radius;
        float a2 = m_levels[m_targetLevel - 1].angle;

        float smoothT = m_transitionProgress * m_transitionProgress * (3.0f - 2.0f * m_transitionProgress);
        
        float currentR = r1 + (r2 - r1) * smoothT;
        
        float diff = a2 - a1;
        while (diff < -M_PI) diff += 2.0f * M_PI;
        while (diff > M_PI) diff -= 2.0f * M_PI;
        float currentA = a1 + diff * smoothT;

        int ex = cx + static_cast<int>(currentR * std::cos(currentA));
        int ey = cy + static_cast<int>(currentR * std::sin(currentA));

        m_canvas->fillCircle(ex, ey, 5, COLOR_ELECTRON);
        m_canvas->drawCircle(ex, ey, 10, COLOR_ELECTRON_GLOW);

        // Calculate Wavy Photon trajectory
        bool isAbsorption = (m_targetLevel > m_sourceLevel);
        float rayLength = 60.0f;
        float photonCenterR;
        
        if (isAbsorption) {
            // Photon rushes IN from the outside to hit the electron
            float startR = r2 + 250.0f; 
            photonCenterR = startR + (r2 - startR) * smoothT; 
        } else {
            // Photon shoots OUT from the electron into space
            float endR = r1 + 250.0f;
            photonCenterR = r1 + (endR - r1) * smoothT;
        }
        
        float px1 = cx + (photonCenterR + rayLength/2) * std::cos(currentA);
        float py1 = cy + (photonCenterR + rayLength/2) * std::sin(currentA);
        float px2 = cx + (photonCenterR - rayLength/2) * std::cos(currentA);
        float py2 = cy + (photonCenterR - rayLength/2) * std::sin(currentA);
        
        // Draw the oscillating sine wave packet
        float dx = px2 - px1;
        float dy = py2 - py1;
        float len = std::sqrt(dx*dx + dy*dy);
        
        if (len > 0.1f) {
            float nx = -dy / len;
            float ny = dx / len;
            int prevX = static_cast<int>(px1);
            int prevY = static_cast<int>(py1);
            int segments = 24;
            
            for(int i = 1; i <= segments; ++i) {
                float segT = static_cast<float>(i) / segments;
                float baseX = px1 + dx * segT;
                float baseY = py1 + dy * segT;
                // Sine wave formula for the light photon
                float wave = std::sin(segT * M_PI * 5.0f) * 6.0f; 
                int wx = static_cast<int>(baseX + nx * wave);
                int wy = static_cast<int>(baseY + ny * wave);
                
                m_canvas->drawLine(prevX, prevY, wx, wy, COLOR_PHOTON);
                prevX = wx; 
                prevY = wy;
            }
        }
    }
}

void BohrStage::setPaused(bool paused) {
    m_paused = paused;
}

void BohrStage::setSpeed(float speed) {
    m_speedMultiplier = speed;
}

void BohrStage::reset() {
    m_currentLevel = 1;
    m_transitioning = false;
    m_transitionProgress = 0.0f;
    m_speedMultiplier = 1.0f;
    
    m_levels.clear();
    // Calculate mathematically accurate radii and velocities
    for (int n = 1; n <= 4; ++n) {
        OrbitLevel level;
        level.radius = 20.0f * (n * n);       // Radius = n^2
        level.angle = 0.0f;
        level.speed = 3.0f / (n * n * n);     // Angular velocity = 1/n^3
        m_levels.push_back(level);
    }
}

void BohrStage::setParam(const std::string& name, float value) {
    if (name == "level") {
        int newLevel = std::max(1, std::min(4, static_cast<int>(value)));
        if (newLevel != m_currentLevel && !m_transitioning) {
            m_sourceLevel = m_currentLevel;
            m_targetLevel = newLevel;
            m_transitioning = true;
            m_transitionProgress = 0.0f;
        }
    }
}

void BohrStage::trigger(const std::string& name) {
    if (name == "photonJump" && !m_transitioning) {
        m_sourceLevel = m_currentLevel;
        m_targetLevel = (m_currentLevel < 4) ? m_currentLevel + 1 : 1;
        m_transitioning = true;
        m_transitionProgress = 0.0f;
    }
}

void BohrStage::onMouseDrag(int dx, int dy) {
    (void)dx; (void)dy;
}

void BohrStage::onScroll(double delta) {
    (void)delta;
}