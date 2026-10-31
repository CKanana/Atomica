#pragma once

#include <string>
#include <utility>

#include "core/Stage.h"
#include "ui/ICanvas.h"
#include "ui/Types.h"

// Stand-in until a real stage exists: draws nothing. All of the R1 virtuals are
// safe no-ops so hidden stages never touch the GL context or allocate anything.
#include <glad/gl.h>

#include "core/Stage.h"

// Stand-in until a real stage exists: fills the screen with one colour.
class PlaceholderStage : public Stage {
public:
    PlaceholderStage(std::string name, float r, float g, float b)
        : name_(std::move(name)), r_(r), g_(g), b_(b) {}

    const char* name() const override { return name_.c_str(); }

    void render(atomica::ui::ICanvas& /*canvas*/, const atomica::ui::Rect& /*viewport*/) override {}

    void setPaused(bool /*paused*/) override {}
    void setSpeed(float /*speed*/) override {}
    void reset() override {}
    void setParam(const std::string& /*name*/, float /*value*/) override {}
    void trigger(const std::string& /*name*/) override {}
    void onMouseDrag(int /*dx*/, int /*dy*/) override {}
    void onScroll(double /*delta*/) override {}
    void render(int, int) override {
        glClearColor(r_, g_, b_, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

private:
    std::string name_;
    float r_, g_, b_;
};
};
