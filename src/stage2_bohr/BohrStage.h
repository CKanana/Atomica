#pragma once

#include "core/Stage.h"
#include "ui/ICanvas.h"
#include <vector>
#include <string>

class BohrStage : public Stage {
public:
    BohrStage(atomica::ui::ICanvas* canvas);
    ~BohrStage() override = default;

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
    struct OrbitLevel {
        float radius;
        float angle;
        float speed;
    };

    atomica::ui::ICanvas* m_canvas;
    bool m_paused = false;
    float m_speedMultiplier = 1.0f;
    float m_nucleusRadius = 16.0f;
    
    // State machine for tracking quantum jumps
    int m_currentLevel = 1;
    int m_sourceLevel = 1;
    int m_targetLevel = 1;
    
    bool m_transitioning = false;
    float m_transitionProgress = 0.0f;

    std::vector<OrbitLevel> m_levels;
};