#pragma once

#include <string>
#include <utility>

#include <glad/gl.h>

#include "core/Stage.h"

// Stand-in until a real stage exists: fills the screen with one colour.
class PlaceholderStage : public Stage {
public:
    PlaceholderStage(std::string name, float r, float g, float b)
        : name_(std::move(name)), r_(r), g_(g), b_(b) {}

    const char* name() const override { return name_.c_str(); }

    void render(int, int) override {
        glClearColor(r_, g_, b_, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

private:
    std::string name_;
    float r_, g_, b_;
};