#pragma once

#include "core/Stage.h"
#include "ui/ICanvas.h"
#include <vector>

class ProbabilityCloudsStage : public Stage {
public:
    ProbabilityCloudsStage(atomica::ui::ICanvas* canvas);
    ~ProbabilityCloudsStage() override = default;

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
    struct CloudNode {
        float x, y, z;
    };

    atomica::ui::ICanvas* m_canvas;
    float m_nucleusRadius;
    float m_pointDensity;
    unsigned int m_seed;
    float m_animPhase;
    std::vector<CloudNode> m_cloud;
};
