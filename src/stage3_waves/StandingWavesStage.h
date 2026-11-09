#pragma once

#include "core/Stage.h"
#include "ui/ICanvas.h"
#include <string>

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
    atomica::ui::ICanvas* m_canvas;
    bool m_paused = false;
    
    float m_nucleusRadius = 16.0f;
    float m_orbitRadius = 120.0f;
    float m_wavePhase = 0.0f;
    int m_currentN = 3; // Quantum level (number of wavelengths)
};