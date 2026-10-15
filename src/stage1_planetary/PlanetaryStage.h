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
    void render(int width, int height) override;

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