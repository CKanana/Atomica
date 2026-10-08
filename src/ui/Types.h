#pragma once

#include <string>
#include <cstdint>
#include <algorithm>

namespace atomica::ui {

/**
 * 2D Integer Point
 */
struct Point2D {
    int x = 0;
    int y = 0;

    constexpr Point2D() = default;
    constexpr Point2D(int px, int py) : x(px), y(py) {}
};

/**
 * 32-bit RGBA Color representation
 */
struct Color {
    uint8_t r = 255;
    uint8_t g = 255;
    uint8_t b = 255;
    uint8_t a = 255;

    constexpr Color() = default;
    constexpr Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255)
        : r(red), g(green), b(blue), a(alpha) {}

    // Static Palette - "Deep Quantum Observatory" theme
    static constexpr Color Background()      { return Color(11, 14, 20); }     // #0B0E14
    static constexpr Color PanelSurface()    { return Color(20, 26, 38); }     // #141A26
    static constexpr Color PanelHeader()     { return Color(28, 36, 52); }     // #1C2434
    static constexpr Color Border()          { return Color(46, 58, 82); }     // #2E3A52
    static constexpr Color BorderActive()    { return Color(0, 229, 255); }    // Quantum Cyan
    
    static constexpr Color TextPrimary()     { return Color(240, 244, 250); }   // Crisp White
    static constexpr Color TextSecondary()   { return Color(148, 163, 184); }   // Muted Slate
    static constexpr Color TextHighlight()   { return Color(0, 229, 255); }    // Quantum Cyan
    
    static constexpr Color ElectronCyan()    { return Color(0, 229, 255); }    // Cyan accent
    static constexpr Color NucleusAmber()    { return Color(251, 191, 36); }   // Warm Gold / Amber
    static constexpr Color DangerRed()       { return Color(248, 113, 113); }  // Failure indicator
    static constexpr Color SuccessGreen()    { return Color(52, 211, 153); }   // Valid state
};

/**
 * Axis-Aligned 2D Bounding Box
 */
struct Rect {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;

    constexpr Rect() = default;
    constexpr Rect(int rx, int ry, int rw, int rh)
        : x(rx), y(ry), width(rw), height(rh) {}

    int right() const { return x + width; }
    int bottom() const { return y + height; }

    bool contains(int px, int py) const {
        return px >= x && px < (x + width) && py >= y && py < (y + height);
    }

    bool contains(const Point2D& pt) const {
        return contains(pt.x, pt.y);
    }
};

/**
 * The 5 historical atomic model stages
 */
enum class StageId : int {
    PLANETARY = 1,
    BOHR = 2,
    STANDING_WAVES = 3,
    PROBABILITY_CLOUDS = 4,
    SHADED_RENDERING = 5
};

/**
 * Detailed historical & theoretical metadata for each stage
 */
struct StageMetadata {
    StageId id;
    std::string title;
    std::string scientist;
    int year;
    std::string coreConcept;
    std::string whyItFailed;
    std::string graphicsTechniques;
};

/**
 * Helper to fetch pedagogical information for any stage
 */
inline StageMetadata getStageMetadata(StageId id) {
    switch (id) {
        case StageId::PLANETARY:
            return {
                StageId::PLANETARY,
                "Planetary Model",
                "Ernest Rutherford",
                1911,
                "Electrons orbit the central positive nucleus in circular paths, resembling a miniature solar system.",
                "Classical electrodynamics mandates that accelerating orbiting charges radiate electromagnetic energy continuously, causing the electron to spiral into the nucleus in ~10^-11 seconds.",
                "Midpoint circle & ellipse algorithms, raster color models"
            };
        case StageId::BOHR:
            return {
                StageId::BOHR,
                "Bohr Model",
                "Niels Bohr",
                1913,
                "Angular momentum is quantized (L = n*hbar). Electrons occupy stationary non-radiating orbits and jump between levels via photon absorption/emission.",
                "Could not explain multi-electron atoms, fine spectral line splitting (Zeeman/Stark effect), or why certain orbits are physically stable.",
                "Discrete orbit rasterization, frame animation, photon particle transitions"
            };
        case StageId::STANDING_WAVES:
            return {
                StageId::STANDING_WAVES,
                "Standing Wave Model",
                "Louis de Broglie",
                1924,
                "Electrons behave as matter waves (lambda = h/p). Allowed orbits occur only where the circumference fits an integer number of wavelengths (n*lambda = 2*pi*r), forming standing waves.",
                "Still fundamentally a 1D/2D circular ring representation. Did not provide a complete 3D spatial probability density or explain subshell geometries.",
                "Parametric curves, Bézier splines, wave deformation animation"
            };
        case StageId::PROBABILITY_CLOUDS:
            return {
                StageId::PROBABILITY_CLOUDS,
                "3D Probability Clouds",
                "Erwin Schrödinger & Max Born",
                1926,
                "The electron does not follow a definite trajectory. Its position is governed by a 3D wavefunction psi(r, theta, phi), where |psi|^2 represents probability density (s and p orbitals).",
                "Raw point clouds lack surface shading, depth occlusion, and realistic volumetric lighting cues.",
                "3D homogeneous transformations, camera view/projection, Monte Carlo point cloud sampling"
            };
        case StageId::SHADED_RENDERING:
            return {
                StageId::SHADED_RENDERING,
                "Shaded Quantum Volume",
                "Modern Computational Physics",
                1930,
                "Fully rendered electron density cloud with realistic illumination, ambient occlusion, depth perception, and smooth transfer functions.",
                "Completed historical evolution for the hydrogen atom application.",
                "Phong/Blinn-Phong lighting, depth shading, volumetric transfer functions, texture mapping"
            };
    }
    return {};
}

} // namespace atomica::ui
