// CloudBuffer.h — GPU storage for a point cloud, with TWO buffers (A = current, B = target).
// The vertex shader blends A -> B with a uniform, which is how one orbital morphs into another
// without any per-frame CPU work.
#pragma once
#include <cstddef>

#include <glad/gl.h>

#include "stage4_clouds/Orbital.h"

class CloudBuffer {
public:
    void init();
    void destroy();

    // Replace the cloud instantly (also used when the point count changes).
    void setImmediate(const OrbitalCloud& cloud);
    // Start a morph toward `cloud`. Falls back to setImmediate if the point count differs.
    // Returns true if a morph is now in progress.
    bool beginMorph(const OrbitalCloud& cloud);
    // Make the morph target the new current cloud (call when the animation has finished).
    void finishMorph();

    bool morphing() const { return morphing_; }
    size_t count() const { return count_; }
    void draw() const;

private:
    void upload(GLuint vbo, const OrbitalCloud& cloud);
    void bindAttribs();

    GLuint vao_ = 0, vboA_ = 0, vboB_ = 0;
    size_t count_ = 0;
    bool morphing_ = false;
};
