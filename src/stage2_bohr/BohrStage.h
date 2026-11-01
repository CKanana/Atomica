#pragma once

#include "core/Stage.h"
#include "ui/ICanvas.h"
#include <vector>

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
    struct Level {
        float radius;
        float angle;
    };

    atomica::ui::ICanvas* m_canvas;
    float m_nucleusRadius;
    int m_level;
    bool m_transitioning;
    float m_photonPhase;
    std::vector<Level> m_levels;
};
