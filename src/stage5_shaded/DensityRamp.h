// DensityRamp.h - colour transfer function for Stage 5.
// Maps probability density t in [0, 1] to an RGBA colour.
//   colour: 4-stop gradient (blue -> purple -> orange -> pale yellow)
//   alpha : t * t  (quadratic falloff, so faint regions fade out fast)
// No OpenGL in here, so it can be tested without a window.
#pragma once
#include <vector>

namespace ramp {

struct Stop { float pos, r, g, b; };

inline std::vector<unsigned char> buildDensityRamp(int size = 256) {
    static const Stop stops[] = {
        {0.00f, 0.10f, 0.22f, 0.95f},
        {0.50f, 0.70f, 0.20f, 0.90f},
        {0.85f, 1.00f, 0.50f, 0.12f},
        {1.00f, 1.00f, 0.95f, 0.75f},
    };
    const int lastSeg = 2;   // 4 stops -> 3 segments (0, 1, 2)

    std::vector<unsigned char> texels(static_cast<size_t>(size) * 4);
    for (int i = 0; i < size; ++i) {
        float t = i / static_cast<float>(size - 1);

        // pick the gradient segment that contains t
        int s = 0;
        while (s < lastSeg && t >= stops[s + 1].pos) ++s;

        float u = (t - stops[s].pos) / (stops[s + 1].pos - stops[s].pos);
        float r = stops[s].r + (stops[s + 1].r - stops[s].r) * u;
        float g = stops[s].g + (stops[s + 1].g - stops[s].g) * u;
        float b = stops[s].b + (stops[s + 1].b - stops[s].b) * u;
        float a = t * t;

        unsigned char* o = &texels[static_cast<size_t>(i) * 4];
        o[0] = static_cast<unsigned char>(255.0f * r + 0.5f);
        o[1] = static_cast<unsigned char>(255.0f * g + 0.5f);
        o[2] = static_cast<unsigned char>(255.0f * b + 0.5f);
        o[3] = static_cast<unsigned char>(255.0f * a + 0.5f);
    }
    return texels;
}

}  // namespace ramp