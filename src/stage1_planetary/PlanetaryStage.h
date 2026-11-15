#pragma once

#include "core/Stage.h"
#include "ui/ICanvas.h"
#include <vector>
#include <string>

class PlanetaryStage : public Stage {
public:
    PlanetaryStage(atomica::ui::ICanvas* canvas);
    ~PlanetaryStage() override = default;

    const char* name() const override;

    void update(double dt) override;
    void render(atomica::ui::ICanvas& canvas, const atomica::ui::Rect& viewport) override;

    void setPaused(bool paused) override;
    void setSpeed(float speed) override;
    void reset() override;
    void setParam(const std::string& name, float value) override;
    void trigger(const std::string& name) override;
    void onMouseDrag(int dx, int dy) override;
    void onScroll(double delta) override;

private:
    struct Orbit {
        float baseRadius;
        float angle;
        float baseSpeed;
    };

    atomica::ui::ICanvas* m_canvas;
    bool m_paused = false;
    
    float m_nucleusRadius = 16.0f;
    
    // UI-controlled parameters
    float m_speedMultiplier = 1.0f;
    float m_radiusScale = 1.0f;  
    float m_eccentricity = 0.0f; 

    std::vector<Orbit> m_orbits;
};