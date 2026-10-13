#pragma once

// Every Atomica stage (planetary, Bohr, waves, clouds, shaded) inherits from this.
// Stage leads: put your subclass in your own folder, e.g. src/stage1_planetary/.
class Stage {
public:
    virtual ~Stage() = default;

    virtual const char* name() const = 0;

    virtual void onEnter() {}                    // called when the user switches TO this stage
    virtual void onExit() {}                     // called when the user switches AWAY
    virtual void update(double /*dt*/) {}        // dt = seconds since last frame
    virtual void render(int width, int height) = 0;
    virtual void onKey(int /*key*/, int /*action*/) {}  // stage-specific keys
};