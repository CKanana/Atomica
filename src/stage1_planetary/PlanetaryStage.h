#pragma once

#include "core/Stage.h"
#include "ui/ICanvas.h"
#include <vector>

class PlanetaryStage : public Stage {
public:
    // Pass the canvas interface so the stage knows where to draw
    explicit PlanetaryStage(atomica::ui::ICanvas* canvas);
    ~PlanetaryStage() override = default;

    const char* name() const override;

    void update(double dt) override;
    void render(atomica::ui::ICanvas& canvas, const atomica::ui::Rect& viewport) override;

    // R1 control hooks (no-op / passthrough) so the stage satisfies the interface.
    void setPaused(bool paused) override;
    void setSpeed(float speed) override;
    void reset() override;
    void setParam(const std::string& name, float value) override;
    void trigger(const std::string& name) override;
    void onMouseDrag(int dx, int dy) override;
    void onScroll(double delta) override;

    // Legacy hook retained for backward-compat / porting: draws into the
    // supplied canvas at the centre of the viewport at the given time t.
    void renderXxx(atomica::ui::ICanvas& canvas, const atomica::ui::Rect& viewport, double t);

private:
    atomica::ui::ICanvas* m_canvas;
    float m_nucleusRadius;

    struct Orbit {
        float radius;
        float angle; // Current rotation in radians
        float speed; // Angular velocity
    };

    std::vector<Orbit> m_orbits;
};