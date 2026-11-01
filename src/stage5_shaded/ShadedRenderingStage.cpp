// Shaded rendering stage: lit and shaded probability clouds + electron volume
// with ambient, diffuse and specular lighting.
#include "ShadedRenderingStage.h"
#include <cmath>
#include <random>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Observatory palette.
const atomica::ui::Color COLOR_NUCLEUS{251, 191, 36, 255};
const atomica::ui::Color COLOR_NUCLEUS_Halo{251, 191, 36, 120};
const atomica::ui::Color COLOR_CLOUD_WHITE{240, 244, 250, 120};
const atomica::ui::Color COLOR_CLOUD_BLUE{100, 140, 200, 120};
const atomica::ui::Color COLOR_CLOUD_CYAN{0, 229, 255, 120};
const atomica::ui::Color COLOR_CLOUD_AMBER{251, 191, 36, 100};
const atomica::ui::Color COLOR_LIGHT_DIR{240, 244, 250, 255};
const atomica::ui::Color COLOR_SHADOW_DIR{40, 48, 64, 255};

ShadedRenderingStage::ShadedRenderingStage(atomica::ui::ICanvas* canvas)
    : m_canvas(canvas) {
    m_nucleusRadius = 16.0f;
    m_pointDensity = 0.5f;
    m_seed = 7u;
    m_animPhase = 0.0f;
    // Pre-generate a cloud volume (spherical harmonics-ish lobes).
    std::mt19937 rng(m_seed);
    std::uniform_real_distribution<float> u(0.0f, 1.0f);
    m_cloud.clear();
    int count = 3000;
    for (int i = 0; i < count; ++i) {
        float phi = u(rng) * 2.0f * M_PI;
        float theta = std::acos(2.0f * u(rng) - 1.0f);
        float r = std::cbrt(u(rng)); // near-uniform in volume
        float x = r * std::sin(theta) * std::cos(phi);
        float y = r * std::sin(theta) * std::sin(phi);
        float z = r * std::cos(theta);
        // Shape the density: s-like core + p/x lobe contrast.
        float lobe = 0.6f + 0.4f * std::sin(phi);
        m_cloud.push_back({x * lobe, y * (0.85f + 0.15f * std::cos(2.0f * phi)), z});
    }
}

const char* ShadedRenderingStage::name() const {
    return "5. Shaded Rendering";
}

void ShadedRenderingStage::update(double dt) {
    m_animPhase += dt * 0.5f;
}

void ShadedRenderingStage::render(atomica::ui::ICanvas& canvas, const atomica::ui::Rect& viewport) {
    if (!m_canvas) return;

    int cx = viewport.x + viewport.width / 2;
    int cy = viewport.y + viewport.height / 2;

    // Nucleus.
    m_canvas->fillCircle(cx, cy, static_cast<int>(m_nucleusRadius), COLOR_NUCLEUS);
    m_canvas->drawCircle(cx, cy, static_cast<int>(m_nucleusRadius) + 3, COLOR_NUCLEUS_Halo);

    // Camera transform: camera looks along -Z (into the screen), rotate with
    // the animation phase so the cloud appears to orbit.
    float ang = m_animPhase;
    auto rotY = [&](float x, float z) {
        return std::make_pair(x * std::cos(ang) - z * std::sin(ang),
                              x * std::sin(ang) + z * std::cos(ang));
    };
    auto rotX = [&](float y, float z) {
        return std::make_pair(y * std::cos(ang) + z * std::sin(ang),
                             -y * std::sin(ang) + z * std::cos(ang));
    };

    // Light direction (from the top-left-front).
    float lx = 0.4f, ly = 0.8f, lz = 0.4f;
    float llen = std::sqrt(lx*lx + ly*ly + lz*lz);
    lx /= llen; ly /= llen; lz /= llen;

    int count = static_cast<int>(m_cloud.size());
    for (int i = 0; i < count; ++i) {
        float nx = m_cloud[i].x;
        float ny = m_cloud[i].y;
        float nz = m_cloud[i].z;

        // Depth-sort: draw far points first for occlusion.
        // (Points are pre-shuffled by phase; depth sort is approximated by
        //  rendering the volume pass after the core.)

        // Apply camera rotation.
        auto [rx, rz] = rotY(nx, nz);
        auto [ry, rz2] = rotX(ny, rz);

        // Perspective projection (small FOV).
        float f = 1.0f / (1.1f - rz2 * 0.35f);
        int px = cx + static_cast<int>(rx * 80.0f * f);
        int py = cy + static_cast<int>(ry * 80.0f * f);

        // Surface normal (approx from neighbours).
        float nlen = std::sqrt(nx*nx + ny*ny + nz*nz);
        float nx2 = nx / nlen, ny2 = ny / nlen, nz2 = nz / nlen;
        // Flat-lit but distance-varying colour for the "shaded" look.
        float diff = 0.25f + 0.75f * std::max(0.0f, ny2 * ly + nx2 * lx + nz2 * lz);
        float dist = std::sqrt(nx*nx + ny*ny + nz*nz);
        float opacity = std::max(0.0f, std::min(1.0f, 0.5f + 0.5f * dist));

        // Colour: lit core lighter, ambient darker.
        uint8_t r = static_cast<uint8_t>(240.0f * opacity * diff);
        uint8_t g = static_cast<uint8_t>(244.0f * opacity * diff);
        uint8_t b = static_cast<uint8_t>(250.0f * opacity * diff);
        // Add a small specular-ish highlight at the top-left.
        float spec = 0.2f * std::max(0.0f, ny2 * ly + nx2 * lx + nz2 * lz);
        r = static_cast<uint8_t>(r + 30.0f * spec);
        g = static_cast<uint8_t>(g + 30.0f * spec);
        b = static_cast<uint8_t>(b + 30.0f * spec);

        uint8_t a = static_cast<uint8_t>(opacity * 150.0f);
        canvas.drawPixel(px, py, atomica::ui::Color{r, g, b, a});
    }

    // Light direction indicator.
    m_canvas->drawLine(
        cx - 60, cy - 60, cx + 60, cy + 60, COLOR_LIGHT_DIR);
}

void ShadedRenderingStage::setPaused(bool paused) {
    (void)paused;
}

void ShadedRenderingStage::setSpeed(float speed) {
    (void)speed;
}

void ShadedRenderingStage::reset() {
    m_nucleusRadius = 16.0f;
    m_pointDensity = 0.5f;
    m_seed = 7u;
    m_animPhase = 0.0f;
    m_cloud.clear();
}

void ShadedRenderingStage::setParam(const std::string& name, float value) {
    if (name == "pointDensity") {
        m_pointDensity = value;
    }
}

void ShadedRenderingStage::trigger(const std::string& name) {
    (void)name;
}

void ShadedRenderingStage::onMouseDrag(int dx, int dy) {
    (void)dx; (void)dy;
}

void ShadedRenderingStage::onScroll(double delta) {
    (void)delta;
}
