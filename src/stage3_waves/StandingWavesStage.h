#pragma once

#include "core/Stage.h"
#include "ui/ICanvas.h"
#include <vector>

class StandingWavesStage : public Stage {
public:
    StandingWavesStage(atomica::ui::ICanvas* canvas);
    ~StandingWavesStage() override = default;

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
    struct WaveNode {
        float radius;
        float angle;
        float n; // number of lobes
    };

    atomica::ui::ICanvas* m_canvas;
    float m_nucleusRadius;
    float m_wavePhase;
    std::vector<WaveNode> m_nodes;
};
