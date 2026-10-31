#pragma once

#include <string>

#include "ui/ICanvas.h"

// Every Atomica stage (planetary, Bohr, waves, clouds, shaded) inherits from this.
// Stage leads: put your subclass in your own folder, e.g. src/stage1_planetary/.
class Stage {
public:
    virtual ~Stage() = default;

    virtual const char* name() const = 0;

    virtual void onEnter() {}                    // called when the user switches TO this stage
    virtual void onExit() {}                     // called when the user switches AWAY
    virtual void update(double /*dt*/) {}        // dt = seconds since last frame

    // --- R1 integration interface ---
    // R1: every frame is a pure function of time. The renderer owns the canvas
    // and the viewport; the stage draws inside the viewport only.
    virtual void render(atomica::ui::ICanvas& canvas, const atomica::ui::Rect& viewport) = 0;

    // Lifecycle / controls wired by main.cpp (UILayoutManager + UIControlPanel).
    virtual void setPaused(bool paused) { m_paused_ = paused; }
    virtual bool getPaused() const { return m_paused_; }
    virtual void setSpeed(float speed) {}                       // 0.1 .. 3.0, 1 = normal
    virtual void reset() {}
    virtual void setParam(const std::string& name, float value) {}
    virtual void trigger(const std::string& name) {}
    virtual void onMouseDrag(int dx, int dy) {}
    virtual void onScroll(double delta) {}

    virtual void onKey(int /*key*/, int /*action*/) {}  // stage-specific keys

private:
    bool m_paused_ = false;
};
